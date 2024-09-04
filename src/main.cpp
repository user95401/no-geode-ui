#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/CCNode.hpp>
class CCNodeGeodeExt : public CCNode {
public:
	void customSetup() {
		if (string::containsAny(this->getID(), { "geode.loader/", "geode-" })) {
			auto parent = this->getParent();
			if (parent && parent->getLayout()) {
				this->removeFromParentAndCleanup(0);
				parent->updateLayout();
			}
		}
	}
	static void setID_det(CCNodeGeodeExt* __this, std::string const& id) {
		__this->setID(id);
		if (string::containsAny(id, { "geode.loader/", "geode-" })) {
			log::warn("{} \"{}\" got removed", __this, id);
			__this->setVisible(0);
			__this->setContentSize(CCSizeZero);
		}
		__this->scheduleOnce(schedule_selector(CCNodeGeodeExt::customSetup), 0.f);
	}
	static void hook() {
		auto hook = Mod::get()->hook(
			reinterpret_cast<void*>(
				geode::addresser::getNonVirtual(&CCNode::setID)
				),
			&CCNodeGeodeExt::setID_det,
			"CCNode::setID",
			tulip::hook::TulipConvention::Thiscall
		);
		if (hook.has_error()) log::error("hook failed: {}", hook.error_or("no error..."));
		if (hook.has_value()) log::debug("{}", hook.value()->getRuntimeInfo().dump(4));
	}
};

$on_mod(Loaded) {
	CCNodeGeodeExt::hook();
};