/*
 * Copyright (C) 2015 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "configutils.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtCore/QLoggingCategory>

QJsonObject ConfigUtils::configuration()
{
    QFile file {configFilePath()};
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(file.readAll(), &error)};
    file.close();

    if (error.error != QJsonParseError::NoError) {
        qCWarning(QLoggingCategory("config-utils")) << "Failure to parse configuration file";
        qCWarning(QLoggingCategory("config-utils")) << "Error" << error.errorString();
        return QJsonObject();
    }

    return document.object();
}

void ConfigUtils::writeConfiguration(std::function<void (QJsonObject &)> &&settingsWriter)
{
    QFile file {configFilePath()};
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(file.readAll(), &error)};

    QJsonObject config {document.object()};
    settingsWriter(config);
    document.setObject(config);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
}

QString ConfigUtils::configFilePath()
{
    QDir dir {QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)};
    if (!dir.exists()) {
        Q_ASSERT(QDir::root().mkpath(dir.absolutePath()));
    }
    const QString &appName {QCoreApplication::instance()->applicationName()};
    if (!dir.exists(appName)) {
        Q_ASSERT(dir.mkdir(appName));
    }
    Q_ASSERT(dir.cd(appName));
    return dir.absoluteFilePath(QLatin1String("config.json"));
}
