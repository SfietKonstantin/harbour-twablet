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

#include <gtest/gtest.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QStandardPaths>

class WrapperObject: public QObject
{
public:
    explicit WrapperObject(int &argc, char **argv)
        : m_argc(argc), m_argv(argv)
    {
    }
    void run()
    {
        QStandardPaths::enableTestMode(true);
        testing::InitGoogleTest(&m_argc, m_argv);
        m_returnCode = RUN_ALL_TESTS();
        QCoreApplication::instance()->postEvent(this, new QEvent(QEvent::User));
    }
protected:
    bool event(QEvent *e) override
    {
        if (e->type() == QEvent::User) {
            QCoreApplication::exit(m_returnCode);
            return true;
        } else {
            return QObject::event(e);
        }
    }
private:
    int &m_argc;
    char **m_argv {nullptr};
    int m_returnCode {0};
};

int main(int argc, char **argv)
{
    QCoreApplication app (argc, argv);
    WrapperObject object {argc, argv};
    object.run();
    return app.exec();
}
