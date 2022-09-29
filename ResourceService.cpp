/***************************************************************************
** MIT License
**
** Copyright (c) 2022 Nikita Vaňků
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
******************************************************************************/

#include "ResourceService.h"

#include <QQmlEngine>

ResourceService::ResourceService(QObject* parent) : QObject{parent} {}

void ResourceService::declareQml() {
    qmlRegisterType<ResourceService>("App", 1, 0, "ResourceService");
}

double ResourceService::pitch() const {
    return m_pitch;
}

double ResourceService::roll() const {
    return m_roll;
}

double ResourceService::yaw() const {
    return m_yaw;
}

void ResourceService::setPitch(double pitch) {
    if (!qFuzzyCompare(m_pitch, pitch)) {
        m_pitch = pitch;
        emit pitchChanged(m_pitch);
    }
}

void ResourceService::setRoll(double roll) {
    if (!qFuzzyCompare(m_roll, roll)) {
        m_roll = roll;
        emit rollChanged(m_roll);
    }
}

void ResourceService::setYaw(double yaw) {
    if (!qFuzzyCompare(m_yaw, yaw)) {
        m_yaw = yaw;
        emit yawChanged(m_yaw);
    }
}
