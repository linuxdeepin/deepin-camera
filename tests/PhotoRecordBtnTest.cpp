/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PhotoRecordBtnTest.h"
#include "src/photorecordbtn.h"
#include "src/videowidget.h"
#include "stub.h"
#include "stub_function.h"
#include "src/capplication.h"
#include "ac-deepin-camera-define.h"
#include "stub/addr_pri.h"
#include "datamanager.h"



PhotoRecordBtnTest::PhotoRecordBtnTest()
{
    mainwindow = CamApp->getMainWindow();
    if (mainwindow){
        btn = mainwindow->findChild<photoRecordBtn *>(BUTTON_PICTURE_VIDEO);
    }
}

PhotoRecordBtnTest::~PhotoRecordBtnTest()
{
    mainwindow = NULL;
    btn = NULL;
}

void PhotoRecordBtnTest::SetUp()
{
   
}

void PhotoRecordBtnTest::TearDown()
{

}

TEST_F(PhotoRecordBtnTest, Photo)
{
    btn->setState(true);

}

TEST_F(PhotoRecordBtnTest, PreRecord)
{
    btn->setState(false);
    btn->setRecordState(photoRecordBtn::preRecord);
}

TEST_F(PhotoRecordBtnTest, Recording)
{
    btn->setState(false);
    btn->setRecordState(photoRecordBtn::Recording);
}

