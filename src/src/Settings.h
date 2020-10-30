#ifndef DC_SETTINGS_H
#define DC_SETTINGS_H

#include <QObject>
#include <QPointer>

#include <DSettingsOption>
#include <DSettingsGroup>
#include <DSettings>
#include "videowidget.h"

namespace dc {
using namespace Dtk::Core;

class Settings: public QObject
{
    Q_OBJECT
public:
    enum Flag {
        ClearWhenQuit,
        ShowThumbnailMode,
        ResumeFromLast,
        AutoSearchSimilar,
        PreviewOnMouseover,
        MultipleInstance,
        PauseOnMinimize,
        HWAccel,
    };

    static Settings &get();
    QString configPath() const
    {
        return _configPath;
    }
    QPointer<DSettings> settings()
    {
        return _settings;
    }

    QPointer<DSettingsGroup> group(const QString &name)
    {
        return settings()->group(name);
    }
    QPointer<DSettingsGroup> shortcuts()
    {
        return group("shortcuts");
    }
    QPointer<DSettingsGroup> base()
    {
        return group("base");
    }
    QPointer<DSettingsGroup> subtitle()
    {
        return group("subtitle");
    }

    QVariant generalOption(const QString &opt);

    void setPathOption(const QString &opt, const QVariant &v);

    QVariant getOption(const QString &opt);

public slots:
    void setNewResolutionList();

signals:
    void shortcutsChanged(const QString &, const QVariant &);
    void baseChanged(const QString &, const QVariant &);
    void subtitleChanged(const QString &, const QVariant &);
    void resolutionchanged(const QString &);//分辨率信号
    void baseMuteChanged(const QString &, const QVariant &);

private:
    Settings();

    QPointer<DSettings> _settings;
    QString _configPath;
};

}

#endif /* ifndef _DMR_SETTINGS_H */
