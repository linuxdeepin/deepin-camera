#ifndef TAKEPHOTOSETTINGAREAWIDGET_H
#define TAKEPHOTOSETTINGAREAWIDGET_H

#include <QWidget>

class QPalette;
class circlePushButton;

class takePhotoSettingAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit takePhotoSettingAreaWidget(QWidget *parent = nullptr);
    void init();
    void showFold(bool bShow);//显示收起按钮
    void showUnfold(bool bShow);//显示展开
    void showDelayButtons(bool bShow);//显示延时拍照按钮
    void showFlashlights(bool bShow);//显示闪光灯按钮
private:
    void initButtons();//初始化所以的按钮
    void initLayout();//按钮位置布局
    void hideAll();//隐藏所有按钮

signals:

public slots:
    void foldBtnClicked();//折叠按钮点击
    void unfoldBtnClicked();//展开按钮点击
    void flashlightFoldBtnClicked();//闪光灯折叠按钮点击
    void flashlightUnfoldBtnClicked();//闪光灯展开按钮点击
    void delayUnfoldBtnClicked();//延时展开按钮点击
    void delayfoldBtnClicked();//延时折叠按钮点击
private:
    circlePushButton        *m_foldBtn;//折叠按钮
    circlePushButton        *m_unfoldBtn;//展开按钮

    circlePushButton        *m_flashlightFoldBtn;//折叠闪光按钮
    circlePushButton        *m_flashlightUnfoldBtn;//展开闪光
    circlePushButton        *m_flashlightOnBtn;//闪光开启按钮
    circlePushButton        *m_flashlightOffBtn;//闪光关闭按钮

    circlePushButton        *m_delayFoldBtn;//折叠延迟拍照
    circlePushButton        *m_delayUnfoldBtn;//展开延迟拍照
    circlePushButton        *m_noDelayBtn;//无延迟
    circlePushButton        *m_delay3SecondBtn;//延迟3s
    circlePushButton        *m_delay6SecondBtn;//延迟6s

    int                      m_butHeightOffset;//按钮之间的间隔


};

#endif // TAKEPHOTOSETTINGAREAWIDGET_H
