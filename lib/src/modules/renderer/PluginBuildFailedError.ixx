export module ddge.modules.renderer.PluginBuildFailedError;

import ddge.modules.app.BuildFailedError;

namespace ddge::renderer {

export class PluginBuildFailedError : public app::BuildFailedError {
public:
    using BuildFailedError::BuildFailedError;
};

}   // namespace ddge::renderer
