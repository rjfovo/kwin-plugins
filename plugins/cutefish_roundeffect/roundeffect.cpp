#include "roundeffect.h"

#include <kwin/opengl/glshadermanager.h>
#include <kwin/opengl/glvertexbuffer.h>
#include <kwin/core/rendertarget.h>

#include <span>
#include <QDebug>

namespace KWin
{

static const QByteArray s_vertexShader = R"(
uniform mat4 modelViewProjectionMatrix;
attribute vec2 vertex;
attribute vec2 texCoord;
varying vec2 fragVertex;
void main(void)
{
    gl_Position = modelViewProjectionMatrix * vec4(vertex, 0.0, 1.0);
    fragVertex = vertex;
}
)";

// stencil 圆角 shader：圆角外 discard（不写 stencil/颜色），圆角内正常输出
static const QByteArray s_stencilFragmentShader = R"(
uniform vec4 box;
uniform vec4 cornerRadius;
varying vec2 fragVertex;

float sdfRoundedBox(vec2 position, vec2 center, vec2 extents, vec4 radius)
{
    vec2 p = position - center;
    float r = p.x > 0.0
        ? (p.y < 0.0 ? radius.y : radius.w)
        : (p.y < 0.0 ? radius.x : radius.z);
    vec2 q = abs(p) - extents + vec2(r);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

void main(void)
{
    float f = sdfRoundedBox(fragVertex, box.xy, box.zw, cornerRadius);
    if (f > 0.0) {
        discard; // 圆角外：丢弃，stencil 保持 0
    }
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); // 圆角内：写 stencil
}
)";

CutefishRoundEffect::CutefishRoundEffect()
{
    m_stencilShader = ShaderManager::instance()->loadShaderFromCode(s_vertexShader, s_stencilFragmentShader);
    if (m_stencilShader) {
        m_mvpLocation = m_stencilShader->uniformLocation("modelViewProjectionMatrix");
        m_boxLocation = m_stencilShader->uniformLocation("box");
        m_cornerRadiusLocation = m_stencilShader->uniformLocation("cornerRadius");
    }
}

CutefishRoundEffect::~CutefishRoundEffect() = default;

bool CutefishRoundEffect::shouldRound(const EffectWindow *w) const
{
    if (w->hasDecoration()) {
        return false;
    }
    if (!w->isNormalWindow() && !w->isDialog()) {
        return false;
    }
    if (w->isFullScreen()) {
        return false;
    }
    return true;
}

void CutefishRoundEffect::setupStencilClip(const RenderTarget &renderTarget, const RenderViewport &viewport,
                                           EffectWindow *w, const WindowPaintData &data)
{
    if (!m_stencilShader) {
        return;
    }

    // 手动附加 stencil renderbuffer 到当前合成 framebuffer
    const QSize fbSize = renderTarget.size();
    if (!m_stencilRbo) {
        glGenRenderbuffers(1, &m_stencilRbo);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, m_stencilRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, fbSize.width(), fbSize.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencilRbo);

    const QRectF frame = w->frameGeometry();
    const qreal scale = viewport.scale();
    const QRectF devGeo(
        (frame.x() + data.xTranslation()) * scale,
        (frame.y() + data.yTranslation()) * scale,
        frame.width() * data.xScale() * scale,
        frame.height() * data.yScale() * scale);
    const qreal radius = 11.0 * scale;
    if (radius <= 0.0) {
        return;
    }

    // 1. 清 stencil
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    // 2. 画圆角到 stencil：圆角内 stencil=1，圆角外 discard（保持 0）
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_BLEND);

    ShaderManager::instance()->pushShader(m_stencilShader.get());
    m_stencilShader->setUniform(m_mvpLocation, viewport.projectionMatrix());
    m_stencilShader->setUniform(m_boxLocation,
                                QVector4D(devGeo.center().x(), devGeo.center().y(),
                                          devGeo.width() * 0.5, devGeo.height() * 0.5));
    m_stencilShader->setUniform(m_cornerRadiusLocation,
                                QVector4D(radius, radius, radius, radius));

    const float x1 = devGeo.left();
    const float y1 = devGeo.top();
    const float x2 = devGeo.right();
    const float y2 = devGeo.bottom();

    GLVertex2D vertices[6] = {
        {QVector2D(x1, y1), QVector2D(0.0f, 0.0f)},
        {QVector2D(x2, y1), QVector2D(1.0f, 0.0f)},
        {QVector2D(x2, y2), QVector2D(1.0f, 1.0f)},
        {QVector2D(x1, y1), QVector2D(0.0f, 0.0f)},
        {QVector2D(x2, y2), QVector2D(1.0f, 1.0f)},
        {QVector2D(x1, y2), QVector2D(0.0f, 1.0f)},
    };

    GLVertexBuffer *vbo = GLVertexBuffer::streamingBuffer();
    vbo->reset();
    vbo->setData(vertices, sizeof(vertices));
    vbo->setAttribLayout(std::span(GLVertexBuffer::GLVertex2DLayout), sizeof(GLVertex2D));
    vbo->bindArrays();
    vbo->draw(GL_TRIANGLES, 0, 6);
    vbo->unbindArrays();

    ShaderManager::instance()->popShader();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // 3. 窗口内容只在 stencil==1（圆角内）绘制
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    GLint stType = GL_NONE;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &stType);
    qDebug() << "RND stencil attach type:" << stType << "rbo:" << m_stencilRbo;
}

void CutefishRoundEffect::drawWindow(const RenderTarget &renderTarget, const RenderViewport &viewport,
                                     EffectWindow *w, int mask, const QRegion &region, WindowPaintData &data)
{
    static int dwCnt = 0;
    if (dwCnt++ % 100 == 0) {
        qDebug() << "RND drawWindow:" << w->windowClass()
                 << "decorated:" << w->hasDecoration()
                 << "normal:" << w->isNormalWindow()
                 << "fullscreen:" << w->isFullScreen();
    }
    const bool round = shouldRound(w);
    if (round) {
        setupStencilClip(renderTarget, viewport, w, data);
    }

    effects->drawWindow(renderTarget, viewport, w, mask, region, data);

    if (round) {
        glDisable(GL_STENCIL_TEST);
    }
}

} // namespace KWin
