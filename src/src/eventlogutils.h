#ifndef EVENTLOGUTILS_H
#define EVENTLOGUTILS_H

#include <QJsonObject>
#include <string>

class EventLogUtils
{
public:
    enum EventTID {
        OpeningTime     = 1000000000,
        ClosingTime     = 1000000001,
        Start           = 1000000003,
        Quit            = 1000000004,
        StartCameraApp  = 1000100020,
        StartPhoto      = 1000100021,
        StartRecording  = 1000100022,
        EndRecording    = 1000100023,
    };

    static EventLogUtils &get();
    void writeLogs(QJsonObject &data);

private:
    bool (*init)(const std::string &packagename, bool enable_sig) = nullptr;
    void (*writeEventLog)(const std::string &eventdata) = nullptr;

    static EventLogUtils *m_instance;

    EventLogUtils();
};

#endif // EVENTLOGUTILS_H
