#ifndef FRMINPUT_H
#define FRMINPUT_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QtSql>
#include <QPushButton>
#include <QTimer>
#include <QShowEvent>
#include <QMutexLocker>

namespace Ui
{
class frmInput;
}

class frmInput : public QWidget
{
    Q_OBJECT

public:
    explicit frmInput(QWidget *parent = 0);
    ~frmInput();

    //单例模式,保证一个程序只存在一个输入法实例对象
    static frmInput *Instance() {
        if (!_m_instance) {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (!_m_instance) {
                _m_instance = new frmInput;
            }
        }
        return _m_instance;
    }

    //初始化面板状态,包括字体大小
    void Init(QString position, QString style, int btnFontSize, int labFontSize);

protected:
    //事件过滤器,处理鼠标在汉字标签处单击操作
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

private:
    Ui::frmInput *ui;
    static frmInput *_m_instance;     //实例对象

    int m_deskWidth;                  //桌面宽度
    int m_deskHeight;                 //桌面高度
    int m_frmWidth;                   //窗体宽度
    int m_frmHeight;                  //窗体高度

    QPoint m_mousePoint;              //鼠标拖动自定义标题栏时的坐标
    bool m_mousePressed;              //鼠标是否按下

    bool m_isPress;                   //是否长按退格键
    QPushButton *m_btnPress;          //长按按钮
    QTimer *m_timerPress;             //退格键定时器
    bool checkPress();              //校验当前长按的按钮

    bool m_isFirst;                   //是否首次加载
    void InitForm();                //初始化窗体数据
    void InitProperty();            //初始化属性
    void ChangeStyle();             //改变样式
    void ChangeFont();              //改变字体大小
    void ShowPanel();               //显示输入法面板

    QWidget *m_currentWidget;         //当前焦点的对象
    QLineEdit *m_currentLineEdit;     //当前焦点的单行文本框
    QTextEdit *m_currentTextEdit;     //当前焦点的多行文本框
    QPlainTextEdit *m_currentPlain;   //当前焦点的富文本框
    QTextBrowser *m_currentBrowser;   //当前焦点的文本浏览框

    QString m_currentEditType;        //当前焦点控件的类型
    QString m_currentPosition;        //当前输入法面板位置类型
    QString m_currentStyle;           //当前输入法面板样式
    int m_btnFontSize;                //当前输入法面板按钮字体大小
    int m_labFontSize;                //当前输入法面板标签字体大小
    void insertValue(QString value);//插入值到当前焦点控件
    void deleteValue();             //删除当前焦点控件的一个字符

    QString m_currentType;            //当前输入法类型
    void changeType(QString type);  //改变输入法类型
    void changeLetter(bool isUpper);//改变字母大小写

    QList<QLabel *>m_labCh;           //汉字标签数组
    QStringList m_allPY;              //所有拼音链表
    QStringList m_currentPY;          //当前拼音链表
    int m_currentPY_index;            //当前拼音索引
    int m_currentPY_count;            //当前拼音数量
    void selectChinese();           //查询汉字
    void showChinese();             //显示查询到的汉字
    void setChinese(int index);     //设置当前汉字
    void clearChinese();            //清空当前汉字信息

};

#endif // FRMINPUT_H
