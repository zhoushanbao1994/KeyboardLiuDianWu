#ifndef FRMNUM_H
#define FRMNUM_H

#include <QWidget>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QTimer>
#include <QMutexLocker>

namespace Ui
{
class frmNum;
}

class frmNum : public QWidget
{
    Q_OBJECT

public:
    explicit frmNum(QWidget *parent = 0);
    ~frmNum();

    //单例模式,保证一个程序只存在一个输入法实例对象
    static frmNum *Instance() {
        if (!_m_instance) {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (!_m_instance) {
                _m_instance = new frmNum;
            }
        }
        return _m_instance;
    }

    //初始化面板状态,包括字体大小
    void Init(QString style, int fontSize);

protected:
    //事件过滤器,处理鼠标按下弹出输入法面板
    bool eventFilter(QObject *obj, QEvent *event);
    //鼠标拖动事件
    void mouseMoveEvent(QMouseEvent *e);
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *e);
    //鼠标松开事件
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    //焦点改变事件槽函数处理
    void focusChanged(QWidget *oldWidget, QWidget *nowWidget);
    //输入法面板按键处理
    void btn_clicked();
    //改变输入法面板样式
    void changeStyle(QString topColor, QString bottomColor,
                     QString borderColor, QString textColor);
    //定时器处理退格键
    void reClicked();
    //显示面板
    void ShowPanel();

private:
    Ui::frmNum *ui;
    static frmNum *_m_instance;       //实例对象

    int m_deskWidth;                  //桌面宽度
    int m_deskHeight;                 //桌面高度
    int m_frmWidth;                   //窗体宽度
    int m_frmHeight;                  //窗体高度

    bool m_isPress;                   //是否长按退格键
    QPushButton *m_btnPress;          //长按按钮
    QTimer *m_timerPress;             //退格键定时器
    bool checkPress();              //校验当前长按的按钮

    QPoint m_mousePoint;              //鼠标拖动自定义标题栏时的坐标
    bool m_mousePressed;              //鼠标是否按下

    bool m_isFirst;                   //是否首次加载
    void InitForm();                //初始化窗体数据
    void InitProperty();            //初始化属性
    void ChangeStyle();             //改变样式

    QWidget *m_currentWidget;         //当前焦点的对象
    QLineEdit *m_currentLineEdit;     //当前焦点的单行文本框
    QTextEdit *m_currentTextEdit;     //当前焦点的多行文本框
    QPlainTextEdit *m_currentPlain;   //当前焦点的富文本框
    QTextBrowser *m_currentBrowser;   //当前焦点的文本浏览框

    QString m_currentEditType;        //当前焦点控件的类型
    QString m_currentStyle;           //当前输入法面板样式
    int m_currentFontSize;            //当前输入法面板字体大小
    void insertValue(QString value);//插入值到当前焦点控件
    void deleteValue();             //删除当前焦点控件的一个字符

};

#endif // FRMNUM_H
