#pragma once

#include <kwin/effect/effect.h>
#include <kwin/effect/effecthandler.h>
#include <kwin/opengl/glshader.h>
#include <kwin/opengl/glframebuffer.h>
#include <kwin/core/renderviewport.h>

#include <QRegion>
#include <memory>

namespace KWin
{

// 用继承提升 GLFramebuffer::initDepthStencilAttachment 的可见性（原为 protected），
// 从而在不修改 KWin 二进制的情况下给合成 framebuffer 附加 stencil。
class PublicFramebuffer : public GLFramebuffer
{
public:
    using GLFramebuffer::initDepthStencilAttachment;
};

class CutefishRoundEffect : public Effect
{
    Q_OBJECT

public:
    CutefishRoundEffect();
    ~CutefishRoundEffect() override;

    void drawWindow(const RenderTarget &renderTarget, const RenderViewport &viewport,
                    EffectWindow *w, int mask, const QRegion &region, WindowPaintData &data) override;

private:
    bool shouldRound(const EffectWindow *w) const;
    void setupStencilClip(const RenderTarget &renderTarget, const RenderViewport &viewport,
                          EffectWindow *w, const WindowPaintData &data);

    std::unique_ptr<GLShader> m_stencilShader;
    GLuint m_stencilRbo = 0;
    int m_mvpLocation = -1;
    int m_boxLocation = -1;
    int m_cornerRadiusLocation = -1;
};

} // namespace KWin
