#include "frminput.h"
#include "ui_frminput.h"
#include "qdesktopwidget.h"

frmInput *frmInput::_m_instance = 0;
frmInput::frmInput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmInput)
{
    ui->setupUi(this);
    this->InitProperty();       // 初始化按键属性
    this->InitForm();           // 初始化窗体数据
    this->ChangeStyle();        // 改变样式
}

frmInput::~frmInput()
{
    delete ui;
}

// 初始化面板状态,包括字体大小
void frmInput::Init(QString position, QString style, int btnFontSize, int labFontSize)
{
    this->m_currentPosition = position; // 当前输入法面板位置类型
    this->m_currentStyle = style;       // 当前输入法面板样式
    this->m_btnFontSize = btnFontSize;  // 当前输入法面板按钮字体大小
    this->m_labFontSize = labFontSize;  // 当前输入法面板标签字体大小
    this->ChangeStyle();                // 改变样式
    this->ChangeFont();                 // 改变字体大小
}

// 鼠标拖动事件
void frmInput::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - m_mousePoint);
        e->accept();
    }
}

// 鼠标按下事件
void frmInput::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

// 鼠标松开事件
void frmInput::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}

void frmInput::InitForm()
{
    // 鼠标是否按下
    this->m_mousePressed = false;
    // 设置窗口属性
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    // 桌面尺寸
    QDesktopWidget w;
    m_deskWidth = w.availableGeometry().width();
    m_deskHeight = w.availableGeometry().height();
    // 窗口尺寸
    m_frmWidth = this->width();
    m_frmHeight = this->height();

    // 加载数据库（汉字）
    QSqlDatabase DbConn;
    DbConn = QSqlDatabase::addDatabase("QSQLITE", "py");
    DbConn.setDatabaseName(qApp->applicationDirPath() + "/py.db");
    DbConn.open();

    m_isFirst = true;
    m_isPress = false;
    m_timerPress = new QTimer(this);
    connect(m_timerPress, SIGNAL(timeout()), this, SLOT(reClicked()));

    m_currentWidget = 0;
    m_currentLineEdit = 0;
    m_currentTextEdit = 0;
    m_currentPlain = 0;
    m_currentBrowser = 0;
    m_currentEditType = "";

    //如果需要更改输入法面板的显示位置,改变currentPosition这个变量即可
    //control--显示在对应输入框的正下方 bottom--填充显示在底部  center--窗体居中显示
    m_currentPosition = "";

    //如果需要更改输入法面板的样式,改变currentStyle这个变量即可
    //blue--淡蓝色  dev--dev风格  black--黑色  brown--灰黑色  lightgray--浅灰色  darkgray--深灰色  gray--灰色  silvery--银色
    m_currentStyle = "";

    //输入法面板字体大小,如果需要更改面板字体大小,该这里即可
    m_btnFontSize = 10;
    m_labFontSize = 10;

    //如果需要更改输入法初始模式,改变currentType这个变量即可
    //min--小写模式  max--大写模式  chinese--中文模式
    m_currentType = "min";
    changeType(m_currentType);

    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton * b, btn) {
        connect(b, SIGNAL(clicked()), this, SLOT(btn_clicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(focusChanged(QWidget *, QWidget *)));
    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}

// 初始化属性
void frmInput::InitProperty()
{
    ui->btnOther1->setProperty("btnOther", true);
    ui->btnOther2->setProperty("btnOther", true);
    ui->btnOther3->setProperty("btnOther", true);
    ui->btnOther4->setProperty("btnOther", true);
    ui->btnOther5->setProperty("btnOther", true);
    ui->btnOther6->setProperty("btnOther", true);
    ui->btnOther7->setProperty("btnOther", true);
    ui->btnOther8->setProperty("btnOther", true);
    ui->btnOther9->setProperty("btnOther", true);
    ui->btnOther10->setProperty("btnOther", true);
    ui->btnOther11->setProperty("btnOther", true);
    ui->btnOther12->setProperty("btnOther", true);
    ui->btnOther13->setProperty("btnOther", true);
    ui->btnOther14->setProperty("btnOther", true);
    ui->btnOther15->setProperty("btnOther", true);
    ui->btnOther16->setProperty("btnOther", true);
    ui->btnOther17->setProperty("btnOther", true);
    ui->btnOther18->setProperty("btnOther", true);
    ui->btnOther19->setProperty("btnOther", true);
    ui->btnOther20->setProperty("btnOther", true);
    ui->btnOther21->setProperty("btnOther", true);

    ui->btnDot->setProperty("btnOther", true);
    ui->btnSpace->setProperty("btnOther", true);
    ui->btnEnter->setProperty("btnOther", true);
    ui->btnDelete->setProperty("btnOther", true);

    ui->btn0->setProperty("btnNum", true);
    ui->btn1->setProperty("btnNum", true);
    ui->btn2->setProperty("btnNum", true);
    ui->btn3->setProperty("btnNum", true);
    ui->btn4->setProperty("btnNum", true);
    ui->btn5->setProperty("btnNum", true);
    ui->btn6->setProperty("btnNum", true);
    ui->btn7->setProperty("btnNum", true);
    ui->btn8->setProperty("btnNum", true);
    ui->btn9->setProperty("btnNum", true);
    ui->btn00->setProperty("btnNum", true);

    ui->btna->setProperty("btnLetter", true);
    ui->btnb->setProperty("btnLetter", true);
    ui->btnc->setProperty("btnLetter", true);
    ui->btnd->setProperty("btnLetter", true);
    ui->btne->setProperty("btnLetter", true);
    ui->btnf->setProperty("btnLetter", true);
    ui->btng->setProperty("btnLetter", true);
    ui->btnh->setProperty("btnLetter", true);
    ui->btni->setProperty("btnLetter", true);
    ui->btnj->setProperty("btnLetter", true);
    ui->btnk->setProperty("btnLetter", true);
    ui->btnl->setProperty("btnLetter", true);
    ui->btnm->setProperty("btnLetter", true);
    ui->btnn->setProperty("btnLetter", true);
    ui->btno->setProperty("btnLetter", true);
    ui->btnp->setProperty("btnLetter", true);
    ui->btnq->setProperty("btnLetter", true);
    ui->btnr->setProperty("btnLetter", true);
    ui->btns->setProperty("btnLetter", true);
    ui->btnt->setProperty("btnLetter", true);
    ui->btnu->setProperty("btnLetter", true);
    ui->btnv->setProperty("btnLetter", true);
    ui->btnw->setProperty("btnLetter", true);
    ui->btnx->setProperty("btnLetter", true);
    ui->btny->setProperty("btnLetter", true);
    ui->btnz->setProperty("btnLetter", true);

    // 汉字标签数组
    m_labCh.append(ui->labCh0);
    m_labCh.append(ui->labCh1);
    m_labCh.append(ui->labCh2);
    m_labCh.append(ui->labCh3);
    m_labCh.append(ui->labCh4);
    m_labCh.append(ui->labCh5);
    m_labCh.append(ui->labCh6);
    m_labCh.append(ui->labCh7);
    m_labCh.append(ui->labCh8);
    m_labCh.append(ui->labCh9);
    for (int i = 0; i < 10; i++) {
        // 安装事件过滤器
        m_labCh[i]->installEventFilter(this);
    }
}

// 显示输入法面板
void frmInput::ShowPanel()
{
    // 设置窗口可见
    this->setVisible(true);
    int width = ui->btn0->width();
    width = width > 60 ? width : 60;
    // 设置按键大小
    ui->btnPre->setMinimumWidth(width);
    ui->btnPre->setMaximumWidth(width);
    ui->btnNext->setMinimumWidth(width);
    ui->btnNext->setMaximumWidth(width);
    ui->btnClose->setMinimumWidth(width);
    ui->btnClose->setMaximumWidth(width);
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool frmInput::eventFilter(QObject *obj, QEvent *event)
{
    /*******************************************************/
    /*********************** 鼠标按下 ***********************/
    /*******************************************************/
    if (event->type() == QEvent::MouseButtonPress) {
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << "按键按下";
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (obj == ui->labCh0) {
                setChinese(0);
            } else if (obj == ui->labCh1) {
                setChinese(1);
            } else if (obj == ui->labCh2) {
                setChinese(2);
            } else if (obj == ui->labCh3) {
                setChinese(3);
            } else if (obj == ui->labCh4) {
                setChinese(4);
            } else if (obj == ui->labCh5) {
                setChinese(5);
            } else if (obj == ui->labCh6) {
                setChinese(6);
            } else if (obj == ui->labCh7) {
                setChinese(7);
            } else if (obj == ui->labCh8) {
                setChinese(8);
            } else if (obj == ui->labCh9) {
                setChinese(9);
            } else if (m_currentEditType != "" && obj != ui->btnClose) {
                QString objName = obj->objectName();
                if (!obj->property("noinput").toBool() && objName != "frmMainWindow"
                        && objName != "frmInputWindow" && objName != "qt_edit_menu") {
                    ShowPanel();
                }
            }

            m_btnPress = (QPushButton *)obj;
            if (checkPress()) {
                m_isPress = true;
                m_timerPress->start(500);
            }
            return false;
        }
    }
    /*******************************************************/
    /*********************** 鼠标松开 ***********************/
    /*******************************************************/
    else if (event->type() == QEvent::MouseButtonRelease) {
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << "按键松开";
        m_btnPress = (QPushButton *)obj;
        if (checkPress()) {
            m_isPress = false;
            m_timerPress->stop();
        }
        return false;
    }
    /*******************************************************/
    /*********************** 键盘按键 ***********************/
    /*******************************************************/
    else if (event->type() == QEvent::KeyPress) {
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << "键盘按键";
        //如果输入法窗体不可见,则不需要处理
        if (!isVisible()) {
            return QWidget::eventFilter(obj, event);
        }

        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        //Shift切换输入法模式,esc键关闭输入法面板,空格取第一个汉字,退格键删除
        //中文模式下回车键取拼音,中文模式下当没有拼音时可以输入空格
        if (keyEvent->key() == Qt::Key_Space) {
            if (ui->labPY->text() != "") {
                setChinese(0);
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            insertValue(ui->labPY->text());
            ui->labPY->setText("");
            selectChinese();
            return true;
        } else if (keyEvent->key() == Qt::Key_Shift) {
            ui->btnType->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Escape) {
            ui->btnClose->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            ui->btnDelete->click();
            return true;
        } else if (keyEvent->text() == "+" || keyEvent->text() == "=") {
            if (ui->labPY->text() != "") {
                ui->btnNext->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->text() == "-" || keyEvent->text() == "_") {
            if (ui->labPY->text() != "") {
                ui->btnPre->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_CapsLock) {
            if (m_currentType != "max") {
                m_currentType = "max";
            } else {
                m_currentType = "min";
            }
            changeType(m_currentType);
            return true;
        } else {
            if (m_currentEditType == "QWidget") {
                return false;
            }
            QString key;
            if (m_currentType == "chinese") {
                key = keyEvent->text();
            } else if (m_currentType == "min") {
                key = keyEvent->text().toLower();
            } else if (m_currentType == "max") {
                key = keyEvent->text().toUpper();
            }
            QList<QPushButton *> btn = this->findChildren<QPushButton *>();
            foreach (QPushButton * b, btn) {
                if (b->text() == key) {
                    b->click();
                    return true;
                }
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

bool frmInput::checkPress()
{
    //只有属于输入法键盘的合法按钮才继续处理
    bool num_ok = m_btnPress->property("btnNum").toBool();
    bool other_ok = m_btnPress->property("btnOther").toBool();
    bool letter_ok = m_btnPress->property("btnLetter").toBool();
    if (num_ok || other_ok || letter_ok) {
        return true;
    }
    return false;
}

// 定时器处理退格键
void frmInput::reClicked()
{
    if (m_isPress) {
        m_timerPress->setInterval(30);
        m_btnPress->click();
    }
}

// 焦点改变事件槽函数处理
void frmInput::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    //qDebug() << "oldWidget:" << oldWidget << " nowWidget:" << nowWidget;
    if (nowWidget != 0 && !this->isAncestorOf(nowWidget)) {
        //在Qt5和linux系统中(嵌入式linux除外),当输入法面板关闭时,焦点会变成无,然后焦点会再次移到焦点控件处
        //这样导致输入法面板的关闭按钮不起作用,关闭后马上有控件获取焦点又显示.
        //为此,增加判断,当焦点是从有对象转为无对象再转为有对象时不要显示.
        //这里又要多一个判断,万一首个窗体的第一个焦点就是落在可输入的对象中,则要过滤掉
#ifndef __arm__
        if (oldWidget == 0x0 && !m_isFirst) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }
#endif

        //如果对应属性noinput为真则不显示
        if (nowWidget->property("noinput").toBool()) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }

        m_isFirst = false;
        if (nowWidget->inherits("QLineEdit")) {
            m_currentLineEdit = (QLineEdit *)nowWidget;
            m_currentEditType = "QLineEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QTextEdit")) {
            m_currentTextEdit = (QTextEdit *)nowWidget;
            m_currentEditType = "QTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            m_currentPlain = (QPlainTextEdit *)nowWidget;
            m_currentEditType = "QPlainTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QTextBrowser")) {
            m_currentBrowser = (QTextBrowser *)nowWidget;
            m_currentEditType = "QTextBrowser";
            ShowPanel();
        } else if (nowWidget->inherits("QComboBox")) {
            QComboBox *cbox = (QComboBox *)nowWidget;
            //只有当下拉选择框处于编辑模式才可以输入
            if (cbox->isEditable()) {
                m_currentLineEdit = cbox->lineEdit() ;
                m_currentEditType = "QLineEdit";
                ShowPanel();
            }
        } else if (nowWidget->inherits("QSpinBox") ||
                   nowWidget->inherits("QDoubleSpinBox") ||
                   nowWidget->inherits("QDateEdit") ||
                   nowWidget->inherits("QTimeEdit") ||
                   nowWidget->inherits("QDateTimeEdit")) {
            m_currentWidget = nowWidget;
            m_currentEditType = "QWidget";
            ShowPanel();
        } else {
            //需要将输入法切换到最初的原始状态--小写,同时将之前的对象指针置为零
            m_currentWidget = 0;
            m_currentLineEdit = 0;
            m_currentTextEdit = 0;
            m_currentPlain = 0;
            m_currentBrowser = 0;
            m_currentEditType = "";
            m_currentType = "min";
            changeType(m_currentType);
            this->setVisible(false);
        }

        //根据用户选择的输入法位置设置-居中显示-底部填充-显示在输入框正下方
        if (m_currentPosition == "center") {
            QPoint pos = QPoint(m_deskWidth / 2 - m_frmWidth / 2, m_deskHeight / 2 - m_frmHeight / 2);
            this->setGeometry(pos.x(), pos.y(), m_frmWidth, m_frmHeight);
        } else if (m_currentPosition == "bottom") {
            this->setGeometry(0, m_deskHeight - m_frmHeight, m_deskWidth, m_frmHeight);
        } else if (m_currentPosition == "control") {
            QRect rect = nowWidget->rect();
            QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
            pos = nowWidget->mapToGlobal(pos);

            int x = pos.x();
            int y = pos.y();
            if (pos.x() + m_frmWidth > m_deskWidth) {
                x = m_deskWidth - m_frmWidth;
            }
            if (pos.y() + m_frmHeight > m_deskHeight) {
                y = pos.y() - m_frmHeight - rect.height() - 35;
            }

            this->setGeometry(x, y, m_frmWidth, m_frmHeight);
        }
    }
}

// 改变输入法类型
void frmInput::changeType(QString type)
{
    if (type == "max") {
        changeLetter(true);
        ui->btnType->setText("大写");
        ui->lab_Title->setText("  中文输入法--大写");
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
    } else if (type == "min") {
        changeLetter(false);
        ui->btnType->setText("小写");
        ui->lab_Title->setText("  中文输入法--小写");
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
    } else {
        changeLetter(false);
        ui->btnType->setText("中文");
        ui->lab_Title->setText("  中文输入法--中文");
        ui->btnOther12->setText("。");
        ui->btnOther14->setText("：");
        ui->btnOther17->setText("，");
        ui->btnOther18->setText("；");
        ui->btnOther21->setText("“");
    }
    //每次切换到模式,都要执行清空之前中文模式下的信息
    clearChinese();
    ui->labPY->setText("");
}

// 改变字母大小写
void frmInput::changeLetter(bool isUpper)
{
    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton * b, btn) {
        if (b->property("btnLetter").toBool()) {
            if (isUpper) {
                b->setText(b->text().toUpper());    // 转为大写
            } else {
                b->setText(b->text().toLower());    // 转为小写
            }
        }
    }
}

// 查询汉字
void frmInput::selectChinese()
{
    clearChinese();
    QSqlQuery query(QSqlDatabase::database("py"));
    QString currentPY = ui->labPY->text();
    QString sql = QString("select word from pinyin where pinyin='%1'").arg(currentPY);
    query.exec(sql);
    //逐个将查询到的字词加入汉字队列
    while(query.next()) {
        QString result = query.value(0).toString();
        QStringList text = result.split(" ");
        foreach (QString txt, text) {
            if (txt.length() > 0) {
                m_allPY.append(txt);
                m_currentPY_count++;
            }
        }
    }
    showChinese();
}

// 显示查询到的汉字
void frmInput::showChinese()
{
    //每个版面最多显示10个汉字
    int count = 0;
    m_currentPY.clear();
    for (int i = 0; i < 10; i++) {
        m_labCh[i]->setText("");
    }

    for (int i = m_currentPY_index; i < m_currentPY_count; i++) {
        if (count == 10) {
            break;
        }
        QString txt = QString("%1.%2").arg(count).arg(m_allPY[m_currentPY_index]);
        m_currentPY.append(m_allPY[m_currentPY_index]);
        m_labCh[count]->setText(txt);
        count++;
        m_currentPY_index++;
    }
    //qDebug() << "currentPY_index:" << currentPY_index << "currentPY_count:" << currentPY_count;
}

// 输入法面板按键处理
void frmInput::btn_clicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (m_currentEditType == "") {
        return;
    }

    QPushButton *btn = (QPushButton *)sender();
    QString objectName = btn->objectName();
    if (objectName == "btnType") {
        if (m_currentType == "min") {
            m_currentType = "max";
        } else if (m_currentType == "max") {
            m_currentType = "chinese";
        } else if (m_currentType == "chinese") {
            m_currentType = "min";
        }
        changeType(m_currentType);
    } else if (objectName == "btnDelete") {
        //如果当前是中文模式,则删除对应拼音,删除完拼音之后再删除对应文本输入框的内容
        if (m_currentType == "chinese") {
            QString txt = ui->labPY->text();
            int len = txt.length();
            if (len > 0) {
                ui->labPY->setText(txt.left(len - 1));
                selectChinese();
            } else {
                deleteValue();
            }
        } else {
            deleteValue();
        }
    } else if (objectName == "btnPre") {
        if (m_currentPY_index >= 20) {
            //每次最多显示10个汉字,所以每次向前的时候索引要减20
            if (m_currentPY_index % 10 == 0) {
                m_currentPY_index -= 20;
            } else {
                m_currentPY_index = m_currentPY_count - (m_currentPY_count % 10) - 10;
            }
        } else {
            m_currentPY_index = 0;
        }
        showChinese();
    } else if (objectName == "btnNext") {
        if (m_currentPY_index < m_currentPY_count - 1) {
            showChinese();
        }
    } else if (objectName == "btnClose") {
        this->setVisible(false);
    } else if (objectName == "btnSpace") {
        if (m_currentType == "chinese") {
            setChinese(0);
        } else {
            insertValue(" ");
        }
    } else if (objectName == "btnEnter") {
        QWidget *widget;
        widget = 0;
        if (m_currentEditType == "QLineEdit") {
            widget = m_currentLineEdit;
        } else if (m_currentEditType == "QTextEdit") {
            widget = m_currentTextEdit;
        } else if (m_currentEditType == "QPlainTextEdit") {
            widget = m_currentPlain;
        } else if (m_currentEditType == "QTextBrowser") {
            widget = m_currentBrowser;
        } else if (m_currentEditType == "QWidget") {
            widget = m_currentWidget;
        }

        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(widget, &keyPress);

        this->setVisible(false);
    } else {
        QString value = btn->text();
        //如果是&按钮，因为对应&被过滤,所以真实的text为去除前面一个&字符
        if (objectName == "btnOther7") {
            value = value.right(1);
        }
        //当前不是中文模式,则单击按钮对应text为传递参数
        if (m_currentType != "chinese") {
            insertValue(value);
        } else {
            //中文模式下,不允许输入特殊字符,单击对应数字按键取得当前索引的汉字
            if (btn->property("btnOther").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                }
            } else if (btn->property("btnNum").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                } else if (objectName == "btn0") {
                    setChinese(0);
                } else if (objectName == "btn1") {
                    setChinese(1);
                } else if (objectName == "btn2") {
                    setChinese(2);
                } else if (objectName == "btn3") {
                    setChinese(3);
                } else if (objectName == "btn4") {
                    setChinese(4);
                } else if (objectName == "btn5") {
                    setChinese(5);
                } else if (objectName == "btn6") {
                    setChinese(6);
                } else if (objectName == "btn7") {
                    setChinese(7);
                } else if (objectName == "btn8") {
                    setChinese(8);
                } else if (objectName == "btn9") {
                    setChinese(9);
                }
            } else if (btn->property("btnLetter").toBool()) {
                ui->labPY->setText(ui->labPY->text() + value);
                selectChinese();
            }
        }
    }
}

// 插入值到当前焦点控件
void frmInput::insertValue(QString value)
{
    if (m_currentEditType == "QLineEdit") {
        m_currentLineEdit->insert(value);
    } else if (m_currentEditType == "QTextEdit") {
        m_currentTextEdit->insertPlainText(value);
    } else if (m_currentEditType == "QPlainTextEdit") {
        m_currentPlain->insertPlainText(value);
    } else if (m_currentEditType == "QTextBrowser") {
        m_currentBrowser->insertPlainText(value);
    } else if (m_currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(value));
        QApplication::sendEvent(m_currentWidget, &keyPress);
    }
}

// 删除当前焦点控件的一个字符
void frmInput::deleteValue()
{
    if (m_currentEditType == "QLineEdit") {
        m_currentLineEdit->backspace();
    } else if (m_currentEditType == "QTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentTextEdit->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QPlainTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentPlain->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QTextBrowser") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentBrowser->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, QString());
        QApplication::sendEvent(m_currentWidget, &keyPress);
    }
}

// 设置当前汉字
void frmInput::setChinese(int index)
{
    int count = m_currentPY.count();
    if (count > index) {
        insertValue(m_currentPY[index]);
        //添加完一个汉字后,清空当前汉字信息,等待重新输入
        clearChinese();
        ui->labPY->setText("");
    }
}

// 清空当前汉字信息
void frmInput::clearChinese()
{
    //清空汉字,重置索引
    for (int i = 0; i < 10; i++) {
        m_labCh[i]->setText("");
    }
    m_allPY.clear();
    m_currentPY.clear();
    m_currentPY_index = 0;
    m_currentPY_count = 0;
}

void frmInput::ChangeStyle()
{
    if (m_currentStyle == "blue") {
        changeStyle("#DEF0FE", "#C0DEF6", "#C0DCF2", "#386487");
    } else if (m_currentStyle == "dev") {
        changeStyle("#C0D3EB", "#BCCFE7", "#B4C2D7", "#324C6C");
    } else if (m_currentStyle == "gray") {
        changeStyle("#E4E4E4", "#A2A2A2", "#A9A9A9", "#000000");
    } else if (m_currentStyle == "lightgray") {
        changeStyle("#EEEEEE", "#E5E5E5", "#D4D0C8", "#6F6F6F");
    } else if (m_currentStyle == "darkgray") {
        changeStyle("#D8D9DE", "#C8C8D0", "#A9ACB5", "#5D5C6C");
    } else if (m_currentStyle == "black") {
        changeStyle("#4D4D4D", "#292929", "#D9D9D9", "#CACAD0");
    } else if (m_currentStyle == "brown") {
        changeStyle("#667481", "#566373", "#C2CCD8", "#E7ECF0");
    } else if (m_currentStyle == "silvery") {
        changeStyle("#E1E4E6", "#CCD3D9", "#B2B6B9", "#000000");
    }
}

// 改变字体大小
void frmInput::ChangeFont()
{
    QFont btnFont(this->font().family(), m_btnFontSize);
    QFont labFont(this->font().family(), m_labFontSize);

    QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        btn->setFont(btnFont);
    }

    QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();
    foreach (QLabel * lab, labs) {
        lab->setFont(labFont);
    }
    ui->btnPre->setFont(labFont);
    ui->btnNext->setFont(labFont);
    ui->btnClose->setFont(labFont);
}

// 改变输入法面板样式
void frmInput::changeStyle(QString topColor, QString bottomColor, QString borderColor, QString textColor)
{
    QStringList qss;
    qss.append(QString("QWidget#widget_title{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append("QPushButton{padding:5px;border-radius:3px;}");
    qss.append(QString("QPushButton:hover{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append(QString("QLabel,QPushButton{color:%1;}").arg(textColor));
    qss.append(QString("QPushButton#btnPre,QPushButton#btnNext,QPushButton#btnClose{padding:5px;}"));
    qss.append(QString("QPushButton{border:1px solid %1;background:rgba(0,0,0,0);}")
               .arg(borderColor));
    qss.append(QString("QLineEdit{border:1px solid %1;border-radius:5px;padding:2px;background:none;selection-background-color:%2;selection-color:%3;}")
               .arg(borderColor).arg(bottomColor).arg(topColor));
    this->setStyleSheet(qss.join(""));
}
