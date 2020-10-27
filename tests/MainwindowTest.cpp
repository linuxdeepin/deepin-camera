#include "MainwindowTest.h"
#include "thumbnailTest.h"


MainwindowTest::MainwindowTest()
{
    QString str = QDir::homePath();
    QString video = str+QString("/Videos/");
    dc::Settings::get().setPathOption("datapath",QVariant("video"));
    dc::Settings::get().settings()->sync();
}

TEST_F(MainwindowTest, mwshow)
{
    CMainWindow* mw = dApp->getMainWindow();
    mw->show();

    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    QTest::qWait(500);

    dc::Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 1);
    //等待新连接相机设备,期间插入USB
//    QTest::qWait(8000);
    //相机切换
    DIconButton* iconBtn=mw->getChangeCam();
    if(iconBtn->isVisible() == true){
        QTest::mouseMove(iconBtn,QPoint(0,0),1000);
        QTest::mousePress(iconBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
        QTest::mouseRelease(iconBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    }
    //拍照
    DButtonBoxButton* iconpixBtn =  mw->getPicBoxBtn();
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    DPushButton* pixBtn = mw->getThubBtn();
    QTest::mouseMove(pixBtn,QPoint(0,0),1000);
    QTest::mousePress(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(1000);

    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 1);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();

    QTest::qWait(500);

    QTest::mouseMove(pixBtn,QPoint(0,0),0);
    QTest::mousePress(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    QTest::mouseRelease(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(500);

    QTest::mouseMove(pixBtn,QPoint(0,0),0);
    QTest::mousePress(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    QTest::mouseRelease(pixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(1000);

    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();

    QTest::qWait(500);

    //录像
    DButtonBoxButton* iconVdBtn =  mw->getVdBoxBtn();
    QTest::mouseMove(iconVdBtn,QPoint(0,0),1000);
    QTest::mousePress(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(3000);

    DPushButton* VdBtn = mw->getThubBtn();
    QTest::mouseMove(VdBtn,QPoint(0,0),1000);
    QTest::mousePress(VdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(VdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);


    QTest::qWait(1000);
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem* imgit;
    if(!it.isEmpty())
        {
            imgit = it.value(0);
            QTest::keyPress(imgit, Qt::Key_Control, Qt::NoModifier,500);//0x01000021 0x43
            QTest::keyPress(imgit, Qt::Key_C, Qt::NoModifier,500);
            QTest::keyRelease(imgit, Qt::Key_Control, Qt::NoModifier,500);
            QTest::keyRelease(imgit, Qt::Key_C, Qt::NoModifier,500);

            QTest::keyPress(imgit, Qt::Key_Delete, Qt::NoModifier,1000);
            QTest::keyRelease(imgit, Qt::Key_Delete, Qt::NoModifier,1000);
        }

    QTest::qWait(3000);
    DPushButton* EndBtn = mw->getEndBtn();
    QTest::mouseMove(EndBtn,QPoint(0,0),1000);
    QTest::mousePress(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    //关闭窗口
    QTest::qWait(3000);


}
TEST_F(MainwindowTest, lastopenpath)
{

}
TEST_F(MainwindowTest, setWindowState1)
{

}

TEST_F(MainwindowTest, setWindowState2)
{

}
