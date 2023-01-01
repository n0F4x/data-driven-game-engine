#include <vector>

class Controller;
class Stage;


class Scheduler {
public:
  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    [[nodiscard]] Scheduler() = default;
    [[nodiscard]] Scheduler(const Scheduler&) = delete;
    [[nodiscard]] Scheduler(Scheduler&&) noexcept = default;

  ///--------------------///
 ///  Member functions  ///
///--------------------///
    void iterate(Controller& controller);
    void add_stage(Stage&& stage);
    void add_render_stage(Stage&& stage);

  ///------------------///
 ///  Static helpers  ///
///------------------///
    static [[nodiscard]] auto empty(Scheduler& scheduler) -> bool;

private:
  ///--------------------///
 ///  Member variables  ///
///--------------------///
    std::vector<Stage> appStages;
    std::vector<Stage> renderStages;
};
