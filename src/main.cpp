#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class AnnouncementPopup : public geode::Popup<std::string const&, std::string const&, std::string const&> {
protected:
    bool setup(std::string const& subject, std::string const& message, std::string const& from) override {
        this->setTitle(subject);

        auto label = CCLabelBMFont::create(message.c_str(), "chatFont.fnt");
        label->limitLabelWidth(m_size.width - 40.f, 0.8f, 0.1f);
        label->setPosition(m_mainLayer->getContentSize() / 2 + ccp(0, 10));
        m_mainLayer->addChild(label);

        auto fromLabel = CCLabelBMFont::create(fmt::format("From: {}", from).c_str(), "goldFont.fnt");
        fromLabel->setScale(0.5f);
        fromLabel->setPosition({m_size.width / 2, 25.f});
        fromLabel->setOpacity(150);
        m_mainLayer->addChild(fromLabel);

        return true;
    }

public:
    static AnnouncementPopup* create(std::string const& subject, std::string const& message, std::string const& from) {
        auto ret = new AnnouncementPopup();
        if (ret && ret->initAnchored(300.f, 200.f, subject, message, from, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto networkRequest = web::WebRequest();
        networkRequest.get("https://your-api-url.com/announcement.json")
            .listen([](web::WebResponse* res) {
                if (res->isSuccess()) {
                    auto data = res->json().value_or(matjson::Object());
                    
                    std::string subject = data["subject"].asString().value_or("Notice");
                    std::string message = data["message"].asString().value_or("");
                    std::string from = data["from"].asString().value_or("RobTop");

                    std::string lastSeen = Mod::get()->getSavedValue<std::string>("last-announcement-id", "");

                    if (lastSeen != subject) {
                        AnnouncementPopup::create(subject, message, from)->show();
                        Mod::get()->setSavedValue("last-announcement-id", subject);
                    }
                } else {
                    FLAlertLayer::create(
                        "Error",
                        "Something Wrong with Announcement Message",
                        "OK"
                    )->show();
                }
            });

        return true;
    }
};
