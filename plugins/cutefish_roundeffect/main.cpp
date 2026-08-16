#include "roundeffect.h"

namespace KWin
{

class CutefishRoundEffectFactory : public EffectPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EffectPluginFactory_iid FILE "metadata.json")
    Q_INTERFACES(KPluginFactory)

public:
    CutefishRoundEffectFactory() = default;

    bool isSupported() const override { return true; }
    bool enabledByDefault() const override { return true; }

    Effect *createEffect() const override { return new CutefishRoundEffect(); }
};

} // namespace KWin

#include "main.moc"
