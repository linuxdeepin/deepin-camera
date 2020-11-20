#include "printhelper.h"
#include "printoptionspage.h"

#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrintPreviewWidget>
#include <QPrinter>
#include <QPainter>
#include <QToolBar>
#include <QCoreApplication>
#include <QImageReader>
#include <QDebug>
#include <dprintpreviewwidget.h>
#include <dprintpreviewdialog.h>

PrintHelper::PrintHelper(QObject *parent)
    : QObject(parent)
{

}


void PrintHelper::showPrintDialog(const QStringList &paths, QWidget *parent)
{
    QList<QImage> imgs;

    for (const QString &path : paths) {
        QImage img(path);

        if (!img.isNull()) {
            imgs << img;
        }
    }
    DPrintPreviewDialog printDialog2(nullptr);
    QObject::connect(&printDialog2, &DPrintPreviewDialog::paintRequested, parent, [ = ](DPrinter * _printer) {
        QPainter painter(_printer);
        for (QImage img : imgs) {
            if (!img.isNull()) {
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                QRect wRect  = _printer->pageRect();
                QImage tmpMap;
                if (img.width() > wRect.width() || img.height() > wRect.height()) {
                    tmpMap = img.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                } else {
                    tmpMap = img;
                }
                QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                                          qreal(wRect.height() - tmpMap.height()) / 2,
                                          tmpMap.width(), tmpMap.height());
                painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                                         tmpMap.height()), tmpMap);
            }
            if (img != imgs.last()) {
                _printer->newPage();
                qDebug() << "painter newPage!    File:" << __FILE__ << "    Line:" << __LINE__;
            }
        }
        painter.end();
    });
    printDialog2.exec();
}

QSize PrintHelper::adjustSize(PrintOptionsPage *optionsPage, QImage img, int resolution, const QSize &viewportSize)
{
    PrintOptionsPage::ScaleMode scaleMode = optionsPage->scaleMode();
    QSize size(img.size());

    if (scaleMode == PrintOptionsPage::ScaleToPage) {
        size.scale(viewportSize, Qt::KeepAspectRatio);
    } else if (scaleMode == PrintOptionsPage::ScaleToExpanding) {
        size.scale(viewportSize, Qt::KeepAspectRatioByExpanding);
    } else if (scaleMode == PrintOptionsPage::ScaleToCustomSize) {
        double imageWidth = optionsPage->scaleWidth();
        double imageHeight = optionsPage->scaleHeight();
        size.setWidth(int(imageWidth * resolution));
        size.setHeight(int(imageHeight * resolution));
    } else {
        const double inchesPerMeter = 100.0 / 2.54;
        int dpmX = img.dotsPerMeterX();
        int dpmY = img.dotsPerMeterY();
        if (dpmX > 0 && dpmY > 0) {
            double wImg = double(size.width()) / double(dpmX) * inchesPerMeter;
            double hImg = double(size.height()) / double(dpmY) * inchesPerMeter;
            size.setWidth(int(wImg * resolution));
            size.setHeight(int(hImg * resolution));
        }
    }
    qDebug() << "adjustSize:" << size <<"    File:" << __FILE__ << "    Line:" << __LINE__;
    return size;
}

QPoint PrintHelper::adjustPosition(PrintOptionsPage *optionsPage, const QSize &imageSize, const QSize &viewportSize)
{
    Qt::Alignment alignment = optionsPage->alignment();
    int posX, posY;

    if (alignment & Qt::AlignLeft) {
        posX = 0;
    } else if (alignment & Qt::AlignHCenter) {
        posX = (viewportSize.width() - imageSize.width()) / 2;
    } else {
        posX = viewportSize.width() - imageSize.width();
    }

    if (alignment & Qt::AlignTop) {
        posY = 0;
    } else if (alignment & Qt::AlignVCenter) {
        posY = (viewportSize.height() - imageSize.height()) / 2;
    } else {
        posY = viewportSize.height() - imageSize.height();
    }
    qDebug() << "adjustPosition X:" << posX << "Y:" << posY <<"    File:" << __FILE__ << "    Line:" << __LINE__;
    return QPoint(posX, posY);
}
