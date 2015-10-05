/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
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

#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtTest/QtTest>
#include <memory>
#include <loadsavemanager.h>

class SimpleLoadSave: public ILoadSave
{
public:
    explicit SimpleLoadSave()
    {
    }
    void load(const QJsonObject &json)
    {
        m_string = json.value(QLatin1String("string")).toString();
        m_double = json.value(QLatin1String("double")).toDouble();
    }
    void save(QJsonObject &json) const
    {
        json.insert(QLatin1String("string"), m_string);
        json.insert(QLatin1String("double"), m_double);
    }
    QString getString() const
    {
        return m_string;
    }
    void setString(const QString &value)
    {
        m_string = value;
    }
    double getDouble() const
    {
        return m_double;
    }
    void setDouble(double value)
    {
        m_double = value;
    }

private:
    QString m_string {};
    double m_double {0.};
};

class TstLoadSaveManager: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        m_loadSaveManager.reset(new LoadSaveManager());
        resetTestCase();
    }

    void testSimple()
    {
        SimpleLoadSave save {};
        save.setString(QLatin1String("test"));
        save.setDouble(12.34);
        QVERIFY(m_loadSaveManager->load(save));
        QVERIFY(m_loadSaveManager->save(save));

        SimpleLoadSave load {};
        QVERIFY(m_loadSaveManager->load(load));
        QCOMPARE(save.getString(), load.getString());
        QCOMPARE(save.getDouble(), load.getDouble());
    }

    void testFailureRoot()
    {
        // Prevent root to be written
        resetTestCase();
        QFile file (rootPath());
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("test");
        file.close();

        SimpleLoadSave save {};
        save.setString(QLatin1String("test"));
        save.setDouble(12.34);
        QVERIFY(!m_loadSaveManager->load(save));
        QVERIFY(!m_loadSaveManager->save(save));
    }

    void testFailureRead1()
    {
        resetTestCase();
        SimpleLoadSave save {};
        save.setString(QLatin1String("test"));
        save.setDouble(12.34);
        QVERIFY(m_loadSaveManager->save(save));

        // Prevent save file to be read
        QFile file (LoadSaveManager::configFilePath());
        file.setPermissions(QFile::Permission());

        QVERIFY(!m_loadSaveManager->load(save));
        QVERIFY(!m_loadSaveManager->save(save));
    }

    void testFailureRead2()
    {
        resetTestCase();
        SimpleLoadSave save {};
        save.setString(QLatin1String("test"));
        save.setDouble(12.34);
        QVERIFY(m_loadSaveManager->save(save));

        // Prevent save file to be read
        QFile file (LoadSaveManager::configFilePath());
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("test");
        file.close();

        QVERIFY(!m_loadSaveManager->load(save));
    }

    void testFailureAppDir()
    {
        // Prevent appdir to be written
        resetTestCase();
        QDir::root().mkpath(rootPath());
        QDir root {rootPath()};
        QFile file (root.absoluteFilePath(QCoreApplication::instance()->applicationName()));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("test");
        file.close();

        SimpleLoadSave save {};
        save.setString(QLatin1String("test"));
        save.setDouble(12.34);
        QVERIFY(!m_loadSaveManager->load(save));
        QVERIFY(!m_loadSaveManager->save(save));
    }

    void cleanupTestCase()
    {
        resetTestCase();
    }
private:
    QString rootPath() const
    {
        return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    }
    void resetTestCase()
    {
        QFileInfo root {rootPath()};
        if (root.exists()) {
            if (root.isFile()) {
                QFile file {root.absoluteFilePath()};
                if (!file.isWritable()) {
                    if (!file.setPermissions(QFile::ReadUser | QFile::WriteUser)) {
                        QFAIL("Something wrong happened when resetting the test-case");
                    }
                }
                file.remove();
            } else if (root.isDir()) {
                QDir dir {root.absoluteFilePath()};
                if (!dir.removeRecursively()) {
                    QFAIL("Something wrong happened when resetting the test-case");
                }
            } else {
                QFAIL("Something wrong happened when resetting the test-case");
            }
        }

        // Recreate the folder that will contain "root"
        if (!QDir::root().mkpath(root.absolutePath())) {
            QFAIL("Something wrong happened when resetting the test-case");
        }
    }

    std::unique_ptr<LoadSaveManager> m_loadSaveManager {};
};

QTEST_MAIN(TstLoadSaveManager)

#include "tst_loadsavemanager.moc"
