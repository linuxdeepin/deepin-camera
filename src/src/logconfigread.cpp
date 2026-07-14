// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logconfigread.h"
#include "ddlog.h"
#include "dtkcore_global.h"
#include "qglobal.h"
#include <QLoggingCategory>
#include <QObject>
#include <DConfig>

DCORE_USE_NAMESPACE
using namespace DDLog;

MLogger::MLogger(QObject *parent)
    : QObject(parent), m_rules(""), m_config(nullptr) {
    QByteArray logRules = qgetenv("QT_LOGGING_RULES");
    // qunsetenv 之前一定不要有任何日志打印，否则取消环境变量设置不会生效
    qunsetenv("QT_LOGGING_RULES");
    // 保存环境变量中的规则
    m_rules = logRules;
    // 从 DConfig 读取日志规则
    m_config = DConfig::create("org.deepin.camera", "org.deepin.camera.encode");
    if (m_config && m_config->isValid()) {
        logRules = m_config->value("rules").toByteArray();
        appendRules(logRules);
        setRules(m_rules);
        // 监听 DConfig 变化，动态更新日志规则
        connect(m_config, &DConfig::valueChanged, this, [this](const QString &key) {
            if (key == "rules") {
                setRules(m_config->value(key).toByteArray());
            }
        });
    }
}

MLogger::~MLogger() {
    if (m_config) {
        m_config->deleteLater();
    }
}

void MLogger::setRules(const QString &rules) {
    auto tmpRules = rules;
    m_rules = tmpRules.replace(";", "\n");
    QLoggingCategory::setFilterRules(m_rules);
}

void MLogger::appendRules(const QString &rules) {
    QString tmpRules = rules;
    tmpRules = tmpRules.replace(";", "\n");
    auto tmplist = tmpRules.split('\n');
    for (int i = 0; i < tmplist.count(); i++) {
        if (m_rules.contains(tmplist.at(i))) {
            tmplist.removeAt(i);
            i--;
        }
    }
    if (tmplist.isEmpty()) {
        return;
    }
    m_rules.isEmpty() ? m_rules = tmplist.join("\n")
                      : m_rules += "\n" + tmplist.join("\n");
}
