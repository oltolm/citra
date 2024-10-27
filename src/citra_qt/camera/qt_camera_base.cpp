// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QMessageBox>
#include "citra_qt/camera/camera_util.h"
#include "citra_qt/camera/qt_camera_base.h"
#include "common/logging/log.h"
#include "core/hle/service/cam/cam.h"

namespace Camera {

QtCameraInterface::QtCameraInterface(Service::CAM::Flip flip) {
    using namespace Service::CAM;
    switch (flip) {
    case Flip::Reverse:
        orient = Qt::Horizontal | Qt::Vertical;
        break;
    case Flip::Vertical:
        orient = Qt::Vertical;
        break;
    case Flip::Horizontal:
        orient = Qt::Vertical;
        break;
    case Flip::None:
        orient = Qt::Orientation(0);
        break;
    }
}

void QtCameraInterface::SetFormat(Service::CAM::OutputFormat output_format) {
    output_rgb = output_format == Service::CAM::OutputFormat::RGB565;
}

void QtCameraInterface::SetResolution(const Service::CAM::Resolution& resolution) {
    width = resolution.width;
    height = resolution.height;
}

void QtCameraInterface::SetFlip(Service::CAM::Flip flip) {
    using namespace Service::CAM;
    switch (flip) {
    case Flip::Reverse:
        orient ^= Qt::Horizontal | Qt::Vertical;
        break;
    case Flip::Vertical:
        orient ^= Qt::Vertical;
        break;
    case Flip::Horizontal:
        orient ^= Qt::Vertical;
        break;
    case Flip::None:
        break;
    }
}

void QtCameraInterface::SetEffect(Service::CAM::Effect effect) {
    if (effect != Service::CAM::Effect::None) {
        LOG_ERROR(Service_CAM, "Unimplemented effect {}", static_cast<int>(effect));
    }
}

std::vector<u16> QtCameraInterface::ReceiveFrame() {
    return CameraUtil::ProcessImage(QtReceiveFrame(), width, height, output_rgb, orient);
}

std::unique_ptr<CameraInterface> QtCameraFactory::CreatePreview(const std::string& config,
                                                                int width, int height,
                                                                Service::CAM::Flip flip) {
    std::unique_ptr<CameraInterface> camera = Create(config, flip);

    if (camera->IsPreviewAvailable()) {
        return camera;
    }
    QMessageBox::critical(
        nullptr, QObject::tr("Error"),
        (config.empty() ? QObject::tr("Couldn't load the camera")
                        : QObject::tr("Couldn't load %1").arg(QString::fromStdString(config))));
    return nullptr;
}

} // namespace Camera
