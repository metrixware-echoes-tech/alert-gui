/* 
 * Gui AlertsWidget.cpp
 * 
 * @author ECHOES Technologies (TSA)
 * @author Guillaume KRIER
 * 
 * @date 20/11/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * AlertsWidget
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "AlertsWidget.h"

using namespace std;

AlertsWidget::AlertsWidget(Echoes::Dbo::Session *session, string apiUrl)
: AbstractPage(session, apiUrl, "alert")
{
    session_ = session;
    apiUrl_ = apiUrl;
    created_ = false;
    newClass_ = false;

    setButtonModif(true);
    setButtonSup(true);
    setLocalTable(true);

    vector_pair_string listTitles;
    listTitles.push_back(make_pair(ETypeJson::text, "name"));
    listTitles.push_back(make_pair(ETypeJson::text, "last_attempt"));
    //    listTitles.push_back(make_pair(ETypeJson::text, "alert_media_specializations"));
    //    listTitles.push_back(make_pair(ETypeJson::text, "al4"));
    setTitles(listTitles);

    lists_string lListUrl;
    list<string> listUrl;

    listUrl.push_back("alerts");
    lListUrl.push_back(listUrl);
    listUrl.clear();

    listUrl.push_back("assets");
    listUrl.push_back("assets/:id/plugins");
    lListUrl.push_back(listUrl);
    listUrl.clear();
    
    // voir si on peut inclure cet appel dans le précédent
    listUrl.push_back("plugins");
    listUrl.push_back("plugins/:id/informations");
    lListUrl.push_back(listUrl);
    listUrl.clear();

    listUrl.push_back("plugins");
    listUrl.push_back("plugins/:id/assets");
    lListUrl.push_back(listUrl);
    listUrl.clear();
    
    listUrl.push_back("informations");
    listUrl.push_back("informations/:id/plugins");
    lListUrl.push_back(listUrl);
    listUrl.clear();
    
    listUrl.push_back("criteria");
    lListUrl.push_back(listUrl);
    listUrl.clear();

    listUrl.push_back("users");
    lListUrl.push_back(listUrl);
    listUrl.clear();
    
    listUrl.push_back("medias");
    lListUrl.push_back(listUrl);
    listUrl.clear();

    setUrl(lListUrl);
}

void AlertsWidget::updatePage(bool getResources)
{
    AbstractPage::updatePage(getResources);
}

vector<string> AlertsWidget::getTitlesTableWidget()
{
    vector<string> titleWidget;
    titleWidget.empty();
    titleWidget.push_back("nb-dest");
    titleWidget.push_back("last-out");
    titleWidget.push_back("modif-dest");
    return titleWidget;
}

vector<string> AlertsWidget::getTitlesTableText()
{
    vector<string> titleText;
    titleText.empty();
    titleText.push_back("name");
    return titleText;
}

vector<long long> AlertsWidget::getIdsTable()
{
    vector<long long> ids;

    try
    {
        Wt::Json::Array& result1 = Wt::Json::Array::Empty;
        Wt::Json::Object tmp;
        Wt::Json::Array::const_iterator idx1;
        if (m_alerts.isNull() == false)
        {
            result1 = m_alerts;
            for (idx1 = result1.begin(); idx1 != result1.end(); idx1++)
            {
                tmp = (*idx1);
                ids.push_back(tmp.get("id"));
            }
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[AlertsWidget] Problems parsing JSON";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[AlertsWidget] JSON Type Exception";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }

    return ids;
}

void AlertsWidget::popupRecipients(string nameAlert, string message)
{
    Wt::WDialog *dialog = new Wt::WDialog();
    dialog->setClosable(true);
    AbstractPage::addButtonsToPopupFooter(dialog);
    dialog->resize(Wt::WLength(750), Wt::WLength(500));
    Wt::WTable *tablePopup = new Wt::WTable(dialog->contents());
    tablePopup->elementAt(0, 0)->addWidget(new Wt::WText(tr("Alert.alert.form.alert")));
    tablePopup->elementAt(0, 1)->addWidget(new Wt::WText(nameAlert));

    tablePopup->elementAt(1, 0)->addWidget(new Wt::WText(tr("Alert.alert.form.rec")));
    Wt::WComboBox *boxUsers = new Wt::WComboBox(tablePopup->elementAt(1, 1));

    tablePopup->elementAt(2, 0)->addWidget(new Wt::WText(tr("Alert.alert.form.media")));
    Wt::WComboBox *boxMedias = new Wt::WComboBox(tablePopup->elementAt(2, 1));

    tablePopup->elementAt(3, 0)->addWidget(new Wt::WText(tr("Alert.alert.form.mess")));
    tablePopup->elementAt(3, 1)->addWidget(tabMessage_);

    for (multimap<long long, pair<long long, string>> ::iterator itU = userInfo_.begin(); itU != userInfo_.end(); itU++)
    {
        boxUsers->addItem((*itU).second.second);
    }

    boxUsers->activated().connect(bind([ = ] ()
    {
        recoverListRecipientAlias((*userInfo_.find(boxUsers->currentIndex())).second.first);
    }));

    for (multimap<long long, pair<pair<long long, long long>, string>>::iterator itM = mediaInfo_.begin(); itM != mediaInfo_.end(); itM++)
    {
        boxMedias->addItem((*itM).second.second);
    }

    Wt::WTable *table = new Wt::WTable(tablePopup->elementAt(2, 2));

    Wt::WLineEdit *time = new Wt::WLineEdit();
    time->setValidator(editValidator(-3));
    table->elementAt(0, 0)->addWidget(new Wt::WText(tr("Alert.alert.form.snooze")));
    table->elementAt(0, 1)->addWidget(time);
    Wt::WText *error = new Wt::WText(tr("Alert.alert.invalid-number"));
    table->elementAt(1, 1)->addWidget(error);
    error->hide();
    
    Wt::WComboBox *comboBox = new Wt::WComboBox();
    comboBox->addItem("min"); // xml
    comboBox->addItem("hour"); // xml
    comboBox->addItem("fois"); // xml
    comboBox->setWidth(Wt::WLength(61));
    table->elementAt(0, 2)->addWidget(comboBox);
    time_ = 0;
    comboBox->changed().connect(bind([ = ] ()
    {
        time_ = comboBox->currentIndex();
    }));

    dialog->finished().connect(bind([ = ] ()
    {
        if (dialog->result() == Wt::WDialog::Rejected)
        {
            closePopup();
            return;
        }
        if (time->validate() == Wt::WValidator::Valid)
        {
            checkPopupRecipients(message, time->text().toUTF8(), (boxMedias->currentIndex() + 1));
            return;
        }
        else
            error->show();
            dialog->show();
    }
    ));
    dialog->show();
}

void AlertsWidget::fillInTabMessage()
{
    if (tabMessage_->count() == 0)
    {
        Wt::WTextArea *test1 = new Wt::WTextArea(messageMailForTab_);
        Wt::WTextArea *test2 = new Wt::WTextArea(messageSmsForTab_);
        Wt::WTextArea *test3 = new Wt::WTextArea(messagePushForTab_);
        tabMessage_->addTab(test1, tr("Alert.alert.form.mail"),
                            Wt::WTabWidget::PreLoading);
        tabMessage_->addTab(test2, tr("Alert.alert.form.sms"),
                            Wt::WTabWidget::PreLoading);
        tabMessage_->addTab(test3, tr("Alert.alert.form.push"),
                            Wt::WTabWidget::PreLoading);

        test1->changed().connect(bind([ = ] (){
                                      messageMailForTab_ = test1->text().toUTF8();
        }));
        test2->changed().connect(bind([ = ] (){
                                      messageSmsForTab_ = test2->text().toUTF8();
        }));
        test3->changed().connect(bind([ = ] (){
                                      messagePushForTab_ = test3->text().toUTF8();
        }));
    }
    else
    {
        ((Wt::WTextArea*)tabMessage_->widget(0))->setText(messageMailForTab_);
        ((Wt::WTextArea*)tabMessage_->widget(1))->setText(messageSmsForTab_);
        ((Wt::WTextArea*)tabMessage_->widget(2))->setText(messagePushForTab_);
    }
}

//                                                                                    ^
//                                                                                    |
// Work popup Two End --------------------------------------------------------------- |

/// ///////////////////////////////////////////// ----
// Work popup One --------------------------------------------------------------- |
//                                                                                |
//                                                                                v

void AlertsWidget::initAlertValueDefinitionPopup(Wt::WTable *tableBox)
{

//    fillInMultiMap();
    getBoxesDatas();

    Wt::WSelectionBox *boxAsset = new Wt::WSelectionBox(tableBox->elementAt(1, 0));
    Wt::WSelectionBox *boxPlugin = new Wt::WSelectionBox(tableBox->elementAt(1, 1));
    Wt::WSelectionBox *boxInfo = new Wt::WSelectionBox(tableBox->elementAt(1, 2));

    keyValue_ = new Wt::WLineEdit(tableBox->elementAt(1, 3));

    boxAsset->resize(Wt::WLength(200), Wt::WLength(150));
    boxAsset->setSelectionMode(Wt::ExtendedSelection);
    boxAsset->setSelectable(true);
//    fillInBox(boxAsset, m_assetBoxNames);

    boxPlugin->resize(Wt::WLength(200), Wt::WLength(150));
    boxPlugin->setSelectionMode(Wt::ExtendedSelection);
//    fillInBox(boxPlugin, m_pluginBoxNames);


    boxInfo->resize(Wt::WLength(150), Wt::WLength(150));
    boxInfo->setSelectionMode(Wt::ExtendedSelection);
    boxInfo->setSelectable(true);
//    fillInBox(boxInfo, m_infoBoxNames);

    assetSelected_.clear();
    pluginSelected_.clear();
    infoSelected_.clear();

}

void AlertsWidget::getBoxesDatas()
{
    getRelatedData(boxType::ASSET);
    getRelatedData(boxType::PLUGIN);
    getRelatedData(boxType::INFORMATION);
}

void AlertsWidget::getRelatedData(int boxType)
{
    switch (boxType)
    {
        case ASSET:
            
            break;
        case PLUGIN:
            break;
        case INFORMATION:
            break;
        default:
            break;
    }
}

// Selected in selectionBox  ----------------


int AlertsWidget::selectItemBox(Wt::WSelectionBox *box, string select)
{
    Wt::WString rst = " ";
    for (int cpt(0); cpt < box->count(); cpt++)
    {
        rst = box->itemText(cpt);
        if (rst.toUTF8().compare(select) == 0)
        {
            set<int> selection;
            selection.clear();
            selection.insert(cpt);
            box->setSelectedIndexes(selection);
            return 0;
        }
    }
    return 1;
}





// ------- init popup one -------

void AlertsWidget::popupAddWidget(Wt::WDialog *dialog, long long id)
{
    tabMessage_ = new Wt::WTabWidget();
    tabMessage_->resize(Wt::WLength(300), Wt::WLength(200));
    recoverListRecipientAlias(this->session_->user().id());
    checkAll_ = 1;
    Wt::WPushButton *ButtonSC = new Wt::WPushButton(tr("Alert.alert.button-save-continu"), dialog->footer());
    ButtonSC->clicked().connect(bind([ = ] (){
                                     checkAll_ = 0;
                                     dialog->accept();
    }));

    dialog->resize(Wt::WLength(750), Wt::WLength(500));

    Wt::WContainerWidget *contain = new Wt::WContainerWidget(dialog->contents());
    Wt::WTable *tableBox = new Wt::WTable(contain);
    Wt::WTableCell *cell0_0 = tableBox->elementAt(0, 0);
    cell0_0->addWidget(new Wt::WText(tr("Alert.alert.add-title-box-asset")));
    Wt::WTableCell *cell0_1 = tableBox->elementAt(0, 1);
    cell0_1->addWidget(new Wt::WText(tr("Alert.alert.add-title-box-plugin")));
    Wt::WTableCell *cell0_2 = tableBox->elementAt(0, 2);
    cell0_2->addWidget(new Wt::WText(tr("Alert.alert.add-title-box-info")));
    Wt::WTableCell *cell0_3 = tableBox->elementAt(0, 3);
    cell0_3->addWidget(new Wt::WText(tr("Alert.alert.add-title-box-key")));

    initAlertValueDefinitionPopup(tableBox);

    errorAsset_ = new Wt::WText(tr("Alert.alert.invalid-select-asset"));
    Wt::WTableCell *cell2_0 = tableBox->elementAt(2, 0);
    cell2_0->addWidget(errorAsset_);

    errorPlugin_ = new Wt::WText(tr("Alert.alert.invalid-select-plugin"));
    Wt::WTableCell *cell2_1 = tableBox->elementAt(2, 1);
    cell2_1->addWidget(errorPlugin_);

    errorInfo_ = new Wt::WText(tr("Alert.alert.invalid-select-info"));
    Wt::WTableCell *cell2_2 = tableBox->elementAt(2, 2);
    cell2_2->addWidget(errorAsset_);


    errorAsset_->hide();
    errorPlugin_->hide();
    errorInfo_->hide();

    // FIXME
    //    new Wt::WText(tr("Alert.alert.add-last-info"), contain);
    //    new Wt::WText("21/12/2012", contain);  //a revoir quand info

    new Wt::WText(tr("Alert.alert.add-compare"), contain); //hide show

    resourcesUnitOne.clear();
    resourcesUnitTwo.clear();

    unitOne_.clear();
    unitTwo_.clear();
    idUnitOne = 1;
    idUnitTwo = 1;

    Wt::WContainerWidget *compareWid = new Wt::WContainerWidget(contain);

    compareBarOne_ = new Wt::WContainerWidget();
    compareBarOne_->addStyleClass("widget-title");
    compareBarTwo_ = new Wt::WContainerWidget();
    compareBarTwo_->addStyleClass("widget-title");

    buttonAddOne_ = new Wt::WPushButton("<i class='icon-plus'></i>");
    buttonAddOne_->setTextFormat(Wt::XHTMLUnsafeText);
    buttonAddOne_->hide();
    buttonAddTwo_ = new Wt::WPushButton("<i class='icon-plus'></i>");
    buttonAddTwo_->setTextFormat(Wt::XHTMLUnsafeText);
    buttonAddTwo_->hide();

    createUnitOne(compareWid);
    createUnitTwo(compareWid);
    createUnitThree(compareWid);

    contain->addWidget(buttonAddOne_);
    contain->addWidget(buttonAddTwo_);
    contain->addWidget(compareBarOne_);
    contain->addWidget(compareBarTwo_);

    compareBarOne_->hide();
    compareBarTwo_->hide();

    buttonAddOne_->clicked().connect(bind([ = ] (){
                                          saveLineEditOne_->show();
                                          createUnitOne(compareWid)->show();
                                          compareBarOne_->show();
    }));

    buttonAddTwo_->clicked().connect(bind([ = ] (){
                                          saveLineEditTwo_->show();
                                          createUnitTwo(compareWid)->show();
                                          compareBarTwo_->show();
    }));


}

// ------------------ Unit ------------------------------

void AlertsWidget::showUnit(long long id)
{
    map<int, Wt::WWidget *>::iterator it;
    hideUnit();
    if (id > 0)
    {
        multimap<long long, long long>::iterator unit = unitsIds_.find(id);
        if ((*unit).second == 1)
        {
            compareBarOne_->show();
            for (it = unitOne_.begin(); it != unitOne_.end(); it++)
            {
                ((Wt::WTable*)(*it).second)->show();
            }
        }
        else if ((*unit).second == 3)
        {
            compareBarTwo_->show();
            for (it = unitTwo_.begin(); it != unitTwo_.end(); it++)
            {
                ((Wt::WTable*)(*it).second)->show();
            }
        }
        else if ((*unit).second == 5) //Enums::EInformationUnitType::boolean
        {
            unitThree_->show();
        }
        else
        {
            Wt::log("warning") << "Error for show table unit";
        }
    }
}

void AlertsWidget::hideUnit()
{
    map<int, Wt::WWidget *>::iterator it;
    for (it = unitOne_.begin(); it != unitOne_.end(); it++)
    {
        ((Wt::WTable*)(*it).second)->hide();
    }
    for (it = unitTwo_.begin(); it != unitTwo_.end(); it++)
    {
        ((Wt::WTable*)(*it).second)->hide();
    }
    unitThree_->hide();
}

Wt::WTable *AlertsWidget::createUnitOne(Wt::WContainerWidget *contain)
{
    Wt::WTable *table = new Wt::WTable(contain);
    Wt::WLineEdit *textEdit = new Wt::WLineEdit(table->elementAt(0, 1));

    Wt::WComboBox *comboBox = new Wt::WComboBox();
    comboBox->addItem("=="); //xml

    table->elementAt(0, 0)->addWidget(comboBox);
    table->hide();

    unitOne_[idUnitOne] = table;

    Wt::WText *errorText = new Wt::WText(tr("Alert.alert.invalid-name-alert"),
                                         table->elementAt(1, 1));
    errorText->hide();

    Wt::WText *text = new Wt::WText(comboBox->currentText());
    compareBarOne_->addWidget(text);

    Wt::WLineEdit *lineEditBar = new Wt::WLineEdit();
    lineEditBar->setWidth(40);
    lineEditBar->hide();
    compareBarOne_->addWidget(lineEditBar);

    saveLineEditOne_ = lineEditBar;

    Wt::WPushButton *buttonValid =
            new Wt::WPushButton("<i class='icon-ok'></i>", table->elementAt(0, 2));
    buttonValid->setStyleClass("btn-dark-warning");
    buttonValid->setTextFormat(Wt::XHTMLUnsafeText);
    buttonValid->clicked().connect(bind([ = ] (){
                                        text->setText(comboBox->currentText() + " \"" + textEdit->text() + "\"");
                                        buttonValid->setText("<i class='icon-ok'></i>");
    }));
    textEdit->changed().connect(bind([ = ] (){text->setText(comboBox->currentText() + " \"" + textEdit->text() + "\"");}));
    comboBox->changed().connect(bind([ = ] (){text->setText(comboBox->currentText() + " \"" + textEdit->text() + "\"");}));

    Wt::WPushButton *buttonDel =
            new Wt::WPushButton("<i class='icon-remove icon-white'></i>", table->elementAt(0, 3));
    buttonDel->setStyleClass("btn-danger");
    buttonDel->setTextFormat(Wt::XHTMLUnsafeText);
    buttonDel->setId(boost::lexical_cast<string>(idUnitOne));
    buttonDel->clicked().connect(bind([ = ] (){
                                      map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, Wt::WComboBox*>>::iterator widUnit =
                                      resourcesUnitOne.find(boost::lexical_cast<int, string>(buttonDel->id()));
                                      (*widUnit).second.first.second->hide(); // errorText
                                      resourcesUnitOne.erase(widUnit);

                                      map<int, Wt::WWidget *>::iterator wid =
                                      unitOne_.find(boost::lexical_cast<int, string>(buttonDel->id()));
                                      contain->removeWidget((*wid).second);
                                      contain->refresh();
                                      unitOne_.erase(wid);

                                      compareBarOne_->removeWidget(text);
                                      compareBarOne_->removeWidget(lineEditBar);
    }));

    if (idUnitOne == 1)
    {
        buttonDel->hide();
    }
    resourcesUnitOne[idUnitOne++] = make_pair(make_pair(textEdit, errorText), comboBox);

    return table;
}

Wt::WTable *AlertsWidget::createUnitTwo(Wt::WContainerWidget *contain)
{
    Wt::WTable *table = new Wt::WTable(contain);
    Wt::WLineEdit *valeurEdit = new Wt::WLineEdit(table->elementAt(0, 2));
    valeurEdit->setValidator(editValidator(-2));

    Wt::WComboBox *comboBox1 = new Wt::WComboBox();
    comboBox1->addItem("<");
    comboBox1->addItem("<=");
    comboBox1->addItem("==");
    comboBox1->addItem("!=");
    comboBox1->addItem(">=");
    comboBox1->addItem(">");

    table->elementAt(0, 1)->addWidget(comboBox1);
    table->elementAt(0, 0)
            ->addWidget(new Wt::WText(tr("Alert.alert.message-value")));

    Wt::WComboBox *comboBox2 = new Wt::WComboBox();
    //    comboBox2->addItem("Ko");
    //    comboBox2->addItem("Mo");
    //    comboBox2->addItem("Go");

    //    table->elementAt(0, 3)->addWidget(comboBox2);

    table->hide();

    unitTwo_[idUnitTwo] = table;

    Wt::WText *errorNumb = new Wt::WText(tr("Alert.alert.invalid-number"),
                                         contain);
    errorNumb->hide();

    Wt::WText *text = new Wt::WText(comboBox1->currentText());
    //            + " \"\" " ;
    //            + comboBox2->currentText());
    //    compareBarTwo_->addWidget(text);

    Wt::WLineEdit *lineEditBar = new Wt::WLineEdit();
    lineEditBar->setWidth(40);
    lineEditBar->hide();
    compareBarTwo_->addWidget(lineEditBar);

    saveLineEditTwo_ = lineEditBar;

    Wt::WPushButton *buttonValid =
            new Wt::WPushButton("<i class='icon-ok'></i>",
                                table->elementAt(0, 4));
    buttonValid->setStyleClass("btn-dark-warning");
    buttonValid->setTextFormat(Wt::XHTMLUnsafeText);
    buttonValid->clicked().connect(bind([ = ] (){
                                        text->setText(comboBox1->currentText() + " \"" + valeurEdit->text());
                                        //        + "\" " + comboBox2->currentText());
                                        buttonValid->setText("<i class='icon-ok'></i>");
    }));
    valeurEdit->changed().connect(bind([ = ] (){text->setText(comboBox1->currentText() + " \"" + valeurEdit->text());
                                       //    + "\" " + comboBox2->currentText()); 
    }));
    comboBox1->changed().connect(bind([ = ] (){text->setText(comboBox1->currentText() + " \"" + valeurEdit->text());
                                      //    + "\" " + comboBox2->currentText()); 
    }));
    //    comboBox2->changed().connect(bind([=] ()
    //    { text->setText(comboBox1->currentText() + " \"" + valeurEdit->text());
    ////    + "\" " + comboBox2->currentText()); 
    //    }));

    Wt::WPushButton *buttonDel =
            new Wt::WPushButton("<i class='icon-remove icon-white'></i>",
                                table->elementAt(0, 5));
    buttonDel->setStyleClass("btn-danger");
    buttonDel->setTextFormat(Wt::XHTMLUnsafeText);
    buttonDel->setId(boost::lexical_cast<string>(idUnitTwo));
    buttonDel->clicked().connect(bind([ = ] (){
                                      map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, pair<Wt::WComboBox*,Wt::WComboBox*>>>::iterator widUnit =
                                      resourcesUnitTwo.find(boost::lexical_cast<int, string>(buttonDel->id()));
                                      (*widUnit).second.first.second->hide(); // errorValue
                                      resourcesUnitTwo.erase(widUnit);

                                      map<int, Wt::WWidget *>::iterator wid =
                                      unitTwo_.find(boost::lexical_cast<int, string>(buttonDel->id()));
                                      contain->removeWidget((*wid).second);
                                      contain->refresh();
                                      unitTwo_.erase(wid);

                                      compareBarTwo_->removeWidget(text);
                                      compareBarTwo_->removeWidget(lineEditBar);
    }));

    if (idUnitTwo == 1)
        buttonDel->hide();
    resourcesUnitTwo[idUnitTwo++] = make_pair(make_pair(valeurEdit, errorNumb), make_pair(comboBox1, comboBox2));

    return table;
}

void AlertsWidget::createUnitThree(Wt::WContainerWidget *contain)
{
    Wt::WTable *table = new Wt::WTable(contain);
    errorBool_ = new Wt::WText(tr("Alert.alert.invalid-choose"), contain);
    errorBool_->hide();

    bool_ = -1;

    table->elementAt(0, 0)->
            addWidget(new Wt::WText(tr("Alert.alert.message-true")));
    Wt::WPushButton *button = new Wt::WPushButton(table->elementAt(0, 0));
    button->setAttributeValue("class", "btn btn-dark-green");
    button->setTextFormat(Wt::XHTMLUnsafeText);
    button->setText("<span class='input-group-btn'><i class='icon-ok icon-white'></i></span>");
    button->clicked().connect(bind([ = ] (){
                                   bool_ = 0;
                                   errorBool_->setText(tr("Alert.alert.message-true"));
                                   errorBool_->show();
    }));

    table->elementAt(0, 1)->
            addWidget(new Wt::WText(tr("Alert.alert.message-false")));
    button = new Wt::WPushButton(table->elementAt(0, 1));
    button->setAttributeValue("class", "btn btn-danger");
    button->setTextFormat(Wt::XHTMLUnsafeText);
    button->setText("<span class='input-group-btn'><i class='icon-remove icon-white'></i></span>");
    button->clicked().connect(bind([ = ] (){
                                   bool_ = 1;
                                   errorBool_->setText(tr("Alert.alert.message-false"));
                                   errorBool_->show();
    }));

    table->hide();

    unitThree_ = table;
}

//                                                                                    ^
//                                                                                    |
// Work popup One End --------------------------------------------------------------- |

void AlertsWidget::modifRecip(long long id)
{
    cout << "modif recipient" << endl;
}

Wt::WValidator *AlertsWidget::editValidator(int who)
{
    Wt::WRegExpValidator *validator = 0;

    //    if (who == -1)
    //        validator->setRegExp(); // pour regex a voir ! 
    if (who == -2)
    {
        validator = new Wt::WRegExpValidator("-?[0-9]+\\.*[0-9]*");
        validator->setMandatory(true);
    }
    if (who == -3)
    {
        validator = new Wt::WRegExpValidator("^[0-9]+");
        validator->setMandatory(true);
    }
    return validator;
}

void AlertsWidget::checkPopupRecipients(string message, string time, int media)
{
    message += "\"alert_media_specialization\":\n[\n";
    //    for (long long cpt(1); cpt <= 3; cpt++)
    //    {
    message += "{\n\"media_id\": " + boost::lexical_cast<string>((*mediaInfo_.find(media - 1)).second.first.first);
    switch (time_)
    {
    case 0:
        message += ",\n\"snooze\": " + boost::lexical_cast<string>((boost::lexical_cast<unsigned int>(time) * 60));
        break;
    case 1:
        message += ",\n\"snooze\": " + boost::lexical_cast<string>((boost::lexical_cast<unsigned int>(time) * 3600));
        break;
    case 2:
        message += ",\n\"snooze\": 0";
        break;
    }
    switch ((*mediaInfo_.find(media - 1)).second.first.second)
    {
    case Enums::EMedia::email:
        message += ",\n\"message\": \"" + messageMailForTab_ + "\"\n}";
        break;
    case Enums::EMedia::sms:
        message += ",\n\"message\": \"" + messageSmsForTab_ + "\"\n}";
        break;
    case Enums::EMedia::mobileapp:
        message += ",\n\"message\": \"" + messagePushForTab_ + "\"\n}";
        break;
    }
    //    }
    message += "\n]\n}";
    postAlertCallApi(message);
    getResourceList();
}


// ici qu'on affiche ou pas les critères de comparaison...

int AlertsWidget::checkInput(vector<Wt::WInteractWidget*> inputName, vector<Wt::WText*> errorMessage)
{
    int checkAll = AbstractPage::checkInput(inputName, errorMessage);
    if (idAll_.first.first < 0 || idAll_.first.second < 0 || idAll_.second.first < 0)
    {
        idAll_.first.first < 0 ? errorAsset_->show() : errorAsset_->hide();
        idAll_.first.second < 0 ? errorPlugin_->show() : errorPlugin_->hide();
        idAll_.second.first < 0 ? errorInfo_->show() : errorInfo_->hide();
        checkAll = 1;
    }
    else
    {
        errorAsset_->hide();
        errorPlugin_->hide();
        errorInfo_->hide();
    }

    switch ((*unitsIds_.find(idAll_.second.first)).second)
    {
        errorsHideOne(resourcesUnitOne);
        errorsHideTwo(resourcesUnitTwo);
        errorBool_->hide();
    case Enums::EInformationUnitType::text:
    {
        for (map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, Wt::WComboBox*>>::iterator it = resourcesUnitOne.begin(); it != resourcesUnitOne.end(); it++)
        {
            /* Mettre en place si validator sur l'input texte.
             if (((Wt::WLineEdit*)(*it).second.first.first)->validate() == Wt::WValidator::Invalid)
            {
                (*it).second.first.second.show();
                checkAll = 1;
                break;
            }
             */
            if (((Wt::WLineEdit*)(*it).second.first.first)->text().toUTF8().size() <= 0)
            {
                (*it).second.first.second->show();
                checkAll = 1;
            }
            else
                (*it).second.first.second->hide();
        }
        break;
    }
    case 3:
    {
        for (map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, pair<Wt::WComboBox*,Wt::WComboBox*>>>::iterator it = resourcesUnitTwo.begin(); it != resourcesUnitTwo.end(); it++)
        {
            if (((Wt::WLineEdit*)(*it).second.first.first)->validate() == Wt::WValidator::Invalid)
            {
                (*it).second.first.second->show();
                checkAll = 1;
            }
            else if (((Wt::WLineEdit*)(*it).second.first.first)->text().toUTF8().size() <= 0)
            {
                (*it).second.first.second->show();
                checkAll = 1;
            }
            else
                (*it).second.first.second->hide();
        }
        break;
    }
    case 5: //Enums::EInformationUnitType::boolean
    {
        if (bool_ < 0)
        {
            errorBool_->show();
            checkAll = 1;
        }
        else
            errorBool_->hide();
        break;
    }
    }
    return checkAll;
}

void AlertsWidget::addResource(vector<Wt::WInteractWidget*> argument)
{

    string message;

    string data = ((Wt::WLineEdit*) * argument.begin())->text().toUTF8();
    boost::algorithm::to_lower(data);
    message += "{\n\"name\": \"" + data + "\",\n";

    switch ((*unitsIds_.find(idAll_.second.first)).second)
    {
    case Enums::EInformationUnitType::text:
    {
        message += "\"alert_criteria_id\": 3,\n";
        message += "\"value\": \"" + (*resourcesUnitOne.begin()).second.first.first->text().toUTF8() + "\",\n";
        //            for (MapUnitOne::iterator itUnit = resourcesUnitOne.begin(); itUnit != resourcesUnitOne.end(); itUnit++)
        //            {
        //                cout
        //                        << "input : " << (*itUnit).second.first.first->text().toUTF8() << endl
        //                        << "Comp : " << (*itUnit).second.second->currentText().toUTF8() << endl;
        //            }
        break;
    }
    case 3:
    {
        message += "\"alert_criteria_id\": " + boost::lexical_cast<string>((*resourcesUnitTwo.begin()).second.second.first->currentIndex() + 1) + ",\n";
        message += "\"value\": \"" + (*resourcesUnitTwo.begin()).second.first.first->text().toUTF8() + "\",\n";
        //            for (MapUnitTwo::iterator itUnit = resourcesUnitTwo.begin(); itUnit != resourcesUnitTwo.end(); itUnit++)
        //            {
        //                cout
        //                        << "input : " << (*itUnit).second.first.first->text().toUTF8() << endl
        //                        << "Comp : " << (*itUnit).second.second.first->currentText().toUTF8() << endl
        //                        << "size : " << (*itUnit).second.second.second->currentText().toUTF8() << endl;
        //            }
        break;
    }
    case 5: //Enums::EInformationUnitType::boolean
    {
        message += "\"alert_criteria_id\": 3,\n";
        if (!(unitThree_->isHidden()))
        {
            if (bool_ == 0)
            {
                message += "\"value\": \"true\",\n";
            }
            else if (bool_ == 1)
            {
                message += "\"value\": \"false\",\n";
            }
            else
                Wt::log("warning") << "Problem for boolean";
        }
        break;
    }
    default:
    {
        message += "\"value\": \"\",\n";
        break;
    }
    }

    message += "\"thread_sleep\": 0,\n";

    message += "\"key_value\": \"" + keyValue_->text().toUTF8() + "\",\n";

    message += "\"asset_id\": " + boost::lexical_cast<string>(idAll_.first.first) + ",\n";
    message += "\"plugin_id\": " + boost::lexical_cast<string>(idAll_.first.second) + ",\n";
    message += "\"information_id\": " + boost::lexical_cast<string>(idAll_.second.first);

    if (checkAll_ == 1)
    {
        message += "\"alert_media_specialization\":\n[\n{\n\"media_id\": "
                + boost::lexical_cast<string>(1)
                + ",\n\"snooze\": 0,\n\"message\": \""
                + messagePushForTab_
                + "\"\n}\n]\n}";

        postAlertCallApi(message);
        getResourceList();
    }
    else
    {
        message += ",\n";
        popupRecipients(data, message);
    }
}

// Utility --------------------------------------------------------

void AlertsWidget::closePopup()
{
    getResourceList();
}

void AlertsWidget::errorsHideOne(map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, Wt::WComboBox*>> error)
{
    for (map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, Wt::WComboBox*>>::iterator it = error.begin(); it != error.end(); it++)
    {
        ((Wt::WText*)(*it).second.first.second)->hide();
    }
}

void AlertsWidget::errorsHideTwo(map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, pair<Wt::WComboBox*,Wt::WComboBox*>>> error)
{
    for (map<long long, pair<pair<Wt::WLineEdit*, Wt::WText*>, pair<Wt::WComboBox*,Wt::WComboBox*>>>::iterator it = error.begin(); it != error.end(); it++)
    {
        ((Wt::WText*)(*it).second.first.second)->hide();
    }
}

void AlertsWidget::close()
{
    delete this;
}

// API CALL AND RETURN --------------------------------------------------

void AlertsWidget::recoverListRecipientAlias(long long userRoleId)
{
    for (long long mediaType(1); mediaType <= 3; mediaType++)
    {
        string apiAddress = this->getApiUrl() + "/informations/"
                + boost::lexical_cast<string>(idAll_.second.first) + "/alias"
                + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
                + "&token=" + session_->user()->token.toUTF8()
                + "&user_role_id=" + boost::lexical_cast<string>(userRoleId)
                + "&media_type_id=" + boost::lexical_cast<string>(mediaType);
        Wt::Http::Client *client = new Wt::Http::Client(this);
        client->done().connect(boost::bind(&AlertsWidget::getAliasInfo, this, _1, _2, userRoleId, mediaType));
        Wt::log("debug") << "AlertsWidget : [GET] address to call : " << apiAddress;
        if (client->get(apiAddress))
            Wt::WApplication::instance()->deferRendering();
        else
            Wt::log("error") << "Error Client Http";




    }
}

void AlertsWidget::clearStructures()
{
    AbstractPage::clearStructures();
    m_alerts = Wt::Json::Value::Null;
    userInfo_.clear();
    mediaInfo_.clear();
    m_assetBoxNames.clear();
    m_pluginBoxNames.clear();
    m_infoBoxNames.clear();
//    m_mapAssetIdsPluginIds.clear();
//    m_mapPluginIdsAssetIds.clear();
//    m_mapPluginIdsInfoIds.clear();
//    m_mapInfoIdsPluginsIds.clear();
    unitsIds_.clear();
    messageMailForTab_.clear();
    messageSmsForTab_.clear();
    messagePushForTab_.clear();
}

void AlertsWidget::postAlertCallApi(string message)
{
    Wt::Http::Message messageAlert;
    messageAlert.addBodyText(message);

    string apiAddress = this->getApiUrl() + "/alerts"
            + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
            + "&token=" + session_->user()->token.toUTF8();

    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->done().connect(boost::bind(&AlertsWidget::postAlert, this, _1, _2));

    Wt::log("debug") << "AlertsWidget : [POST] address to call : " << apiAddress;
    Wt::log("debug") << "[POST] Message : " << messageAlert.body();

    if (client->post(apiAddress, messageAlert))
        Wt::WApplication::instance()->deferRendering();
    else
        Wt::log("error") << "Error Client Http";

}

Wt::WDialog *AlertsWidget::deleteResource(long long id)
{
    Wt::WDialog *box = AbstractPage::deleteResource(id);
    box->show();
    box->finished().connect(bind([ = ] (){
                                 if (box->result() == Wt::WDialog::Accepted)
        {
                                 Wt::Http::Message message;
                                 message.addBodyText("");
                                 string apiAddress = this->getApiUrl()
                                 + "/alerts/" + boost::lexical_cast<string>(id)
                                 + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
                                 + "&token=" + session_->user()->token.toUTF8();

                                 Wt::Http::Client *client = new Wt::Http::Client(this);
                                 client->done().connect(boost::bind(&AlertsWidget::deleteAlert, this, _1, _2));
                                 Wt::log("debug") << "AlertsWidget : [DELETE] address to call : " << apiAddress;
                                 if (client->deleteRequest(apiAddress, message))
                                 Wt::WApplication::instance()->deferRendering();
            else
                                 Wt::log("error") << "Error Client Http";
            }
                                 return box;
    }));
    return box;
}

void AlertsWidget::modifResource(vector<Wt::WInteractWidget*> arguments, long long id)
{
    //    Wt::Http::Message message;

    //    Wt::WLineEdit *test;
    //    test = (Wt::WLineEdit*)(*arguments.begin());

    /*
    message.addBodyText("{\n\"med_id\": " + boost::lexical_cast<string>(this->type_)
            + ",\n\"mev_value\":\"" + boost::lexical_cast<string>(test->text()) + "\"\n}");

    string apiAddress = this->getApiUrl() + "/medias/" + boost::lexical_cast<string>(id);
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->done().connect(boost::bind(&UserEditionWidget::putMedia, this, _1, _2));
    apiAddress += "?login=" + session_->user()->eMail.toUTF8() + "&token=" + session_->user()->token.toUTF8();
    Wt::log("debug") << "AlertsWidget : [PUT] address to call : " << apiAddress;
    if (client->put(apiAddress, message))
        Wt::WApplication::instance()->deferRendering();
    else
        Wt::log("error") << "Error Client Http";
     */
}

void AlertsWidget::handleJsonGet(vectors_Json jsonResources)
{
   
    vector<Wt::Json::Value> jsonResource = jsonResources.at(1);
    try
    {
        if (jsonResource.size() > 0)
        {
            Wt::Json::Array& jsonArray = (*jsonResource.begin());
            if (!jsonArray.empty())
            {
                for (int cpt(0); cpt < (int) jsonArray.size(); cpt++)
                {
                    Wt::Json::Object jsonAsset = jsonArray.at(cpt);
                    Wt::Json::Array jsonAssetPlugins = jsonResource.at(cpt + 1);
                    long long assetId = jsonAsset.get("id");
                    vector<long long> pluginsIds;
                    cout << "ASSET ID " << assetId << endl;
                    for (int cpt1(0); cpt1 < (int) jsonAssetPlugins.size(); cpt1++)
                    {
                        Wt::Json::Object jsonPlugin = jsonAssetPlugins.at(cpt1);
                        long long pluginId = jsonPlugin.get("id");
                        cout << ">> PLUGIN ID " << pluginId << endl;
                        pluginsIds.push_back(pluginId);
                    }
                    m_mapAssetPlugins[assetId] = pluginsIds;
                }
            }
        }
            
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Alerts][AST_PLG] Problems parsing JSON";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Alerts][AST_PLG] JSON Type Exception";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    
    
    // plugins // infos
    jsonResource = jsonResources.at(2);
    try
    {
        if (jsonResource.size() > 0)
        {
            Wt::Json::Array& jsonArray = (*jsonResource.begin());
            if (!jsonArray.empty())
            {
                for (int cpt(0); cpt < (int) jsonArray.size(); cpt++)
                {
                    Wt::Json::Object jsonPlugin = jsonArray.at(cpt);
                    Wt::Json::Array jsonPluginInfos = jsonResource.at(cpt + 1);
                    long long pluginId = jsonPlugin.get("id");
                    vector<long long> infosIds;
                    cout << "PLUGIN ID " << pluginId << endl;
                    for (int cpt1(0); cpt1 < (int) jsonPluginInfos.size(); cpt1++)
                    {
                        Wt::Json::Object jsonInfo = jsonPluginInfos.at(cpt1);
                        long long infoId = jsonInfo.get("id");
                        cout << ">> INFO ID " << infoId << endl;
                        infosIds.push_back(infoId);
                    }
                    m_mapPluginInfos[pluginId] = infosIds;
                }
            }
        }
            
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Alerts][PLG_INF] Problems parsing JSON";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Alerts][PLG_INF] JSON Type Exception";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    
    // plugins // assets
    jsonResource = jsonResources.at(3);
    try
    {
        if (jsonResource.size() > 0)
        {
            Wt::Json::Array& jsonArray = (*jsonResource.begin());
            if (!jsonArray.empty())
            {
                for (int cpt(0); cpt < (int) jsonArray.size(); cpt++)
                {
                    Wt::Json::Object jsonPlugin = jsonArray.at(cpt);
                    Wt::Json::Array jsonPluginAssets = jsonResource.at(cpt + 1);
                    long long pluginId = jsonPlugin.get("id");
                    vector<long long> infosIds;
                    cout << "PLUGIN ID " << pluginId << endl;
                    for (int cpt1(0); cpt1 < (int) jsonPluginAssets.size(); cpt1++)
                    {
                        Wt::Json::Object jsonAsset = jsonPluginAssets.at(cpt1);
                        long long assetId = jsonAsset.get("id");
                        cout << ">> ASSET ID " << assetId << endl;
                        infosIds.push_back(assetId);
                    }
                    m_mapPluginAssets[pluginId] = infosIds;
                }
            }
        }
            
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Alerts][PLG_AST] Problems parsing JSON";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Alerts][PLG_AST] JSON Type Exception";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    
    // informations // plugins
    jsonResource = jsonResources.at(4);
    try
    {
        if (jsonResource.size() > 0)
        {
            Wt::Json::Array& jsonArray = (*jsonResource.begin());
            if (!jsonArray.empty())
            {
                for (int cpt(0); cpt < (int) jsonArray.size(); cpt++)
                {
                    Wt::Json::Object jsonInformation = jsonArray.at(cpt);
                    Wt::Json::Value jsonInformationPlugins = jsonResource.at(cpt + 1);
                    long long infoId = jsonInformation.get("id");
                    vector<long long> pluginsIds;
                    cout << "INFO ID " << infoId << endl;
                    if (jsonInformationPlugins.type() == Wt::Json::ArrayType)
                    {
                        Wt::Json::Array jsonInformationPluginsArray = jsonInformationPlugins;
                        for (int cpt1(0); cpt1 < (int) jsonInformationPluginsArray.size(); cpt1++)
                        {
                            Wt::Json::Object jsonPlugin = jsonInformationPluginsArray.at(cpt1);
                            long long pluginId = jsonPlugin.get("id");
                            cout << ">> PLUGIN ID " << pluginId << endl;
                            pluginsIds.push_back(pluginId);
                        }
                    }
                    m_mapInfoPlugins[infoId] = pluginsIds;
                }
            }
        }
            
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Alerts][INFO_PLG] Problems parsing JSON";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Alerts][INFO_PLG] JSON Type Exception";
        Wt::WMessageBox::show(tr("Alert.asset.database-error-title"), tr("Alert.asset.database-error"), Wt::Ok);
    }
          
    cout << "HANDLE PARENT CLASS" << endl;
    AbstractPage::handleJsonGet(jsonResources);
    cout << "AFTER HANDLE PARENT CLASS" << endl;
}

// API CALLS ------------------------------------------

void AlertsWidget::postAlert(boost::system::error_code err, const Wt::Http::Message& response)
{
    cout << "post Alert reponse : " << response.body() << " Status : " << response.status() << endl;
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() == 201)
        {
            try
            {
                Wt::Json::Value result;
                Wt::Json::parse(response.body(), result);
            }
            catch (Wt::Json::ParseError const& e)
            {
                Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
            }
            catch (Wt::Json::TypeException const& e)
            {
                Wt::log("warning") << "[Alerts Widget] JSON Type Exception: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "TypeException", tr("Alert.alert.database-error"), Wt::Ok);
            }
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;
}


void AlertsWidget::getAliasInfo(boost::system::error_code err, const Wt::Http::Message& response, long long userRoleId, long long mediaType)
{
    cout << response.status() << " get Alias Info : " << response.body() << endl;
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() >= 200 && response.status() < 300)
        {
            try
            {
                Wt::Json::Object tmp;
                Wt::Json::Value result;
                Wt::WString message;
                Wt::Json::parse(response.body(), result);
                tmp = result;
                message = tmp.get("alias");
                switch (mediaType)
                {
                case Enums::EMedia::email:
                    messageMailForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::sms:
                    messageSmsForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::mobileapp:
                    messagePushForTab_ += message.toUTF8() + " ";
                    break;
                }
            }
            catch (Wt::Json::ParseError const& e)
            {
                Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
            }
            catch (Wt::Json::TypeException const& e)
            {
                Wt::log("warning") << "[Alerts Widget] JSON Type Exception: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "TypeException", tr("Alert.alert.database-error"), Wt::Ok);
            }
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;

    string apiAddress = this->getApiUrl() + "/assets/"
            + boost::lexical_cast<string>(idAll_.first.first) + "/alias"
            + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
            + "&token=" + session_->user()->token.toUTF8()
            + "&user_role_id=" + boost::lexical_cast<string>(userRoleId)
            + "&media_type_id=" + boost::lexical_cast<string>(mediaType);
    Wt::Http::Client *client1 = new Wt::Http::Client(this);
    client1->done().connect(boost::bind(&AlertsWidget::getAliasAsset, this, _1, _2, userRoleId, mediaType));
    Wt::log("debug") << "AlertsWidget : [GET] address to call : " << apiAddress;
    if (client1->get(apiAddress))
        Wt::WApplication::instance()->deferRendering();
    else
        Wt::log("error") << "Error Client Http";

}

void AlertsWidget::getAliasAsset(boost::system::error_code err, const Wt::Http::Message& response, long long userRoleId, long long mediaType)
{
    cout << response.status() << " get Alias Asset : " << response.body() << endl;
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() >= 200 && response.status() < 300)
        {
            try
            {
                Wt::Json::Object tmp;
                Wt::Json::Value result;
                Wt::WString message;
                Wt::Json::parse(response.body(), result);
                tmp = result;
                message = tmp.get("alias");
                switch (mediaType)
                {
                case Enums::EMedia::email:
                    messageMailForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::sms:
                    messageSmsForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::mobileapp:
                    messagePushForTab_ += message.toUTF8() + " ";
                    break;
                }
            }
            catch (Wt::Json::ParseError const& e)
            {
                Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
            }
            catch (Wt::Json::TypeException const& e)
            {
                Wt::log("warning") << "[Alerts Widget] JSON Type Exception: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "TypeException", tr("Alert.alert.database-error"), Wt::Ok);
            }
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;

    string criteria_id = "3";
    if (unitsIds_.find(idAll_.second.first)->second == 3)
    {
        criteria_id = boost::lexical_cast<string>(resourcesUnitTwo.begin()->second.second.first->currentIndex() + 1);
    }

    string apiAddress = this->getApiUrl() + "/criteria/"
            + criteria_id + "/alias"
            + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
            + "&token=" + session_->user()->token.toUTF8()
            + "&user_role_id=" + boost::lexical_cast<string>(userRoleId)
            + "&media_type_id=" + boost::lexical_cast<string>(mediaType)
            + "&information_id=" + boost::lexical_cast<string>(idAll_.second.first);
    Wt::Http::Client *client2 = new Wt::Http::Client(this);
    client2->done().connect(boost::bind(&AlertsWidget::getAliasCriteria, this, _1, _2, userRoleId, mediaType));
    Wt::log("debug") << "AlertsWidget : [GET] address to call : " << apiAddress;
    if (client2->get(apiAddress))
        Wt::WApplication::instance()->deferRendering();
    else
        Wt::log("error") << "Error Client Http";

}

void AlertsWidget::getAliasCriteria(boost::system::error_code err, const Wt::Http::Message& response, long long userRoleId, long long mediaType)
{
    cout << response.status() << " get Alias Criteria : " << response.body() << endl;
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() >= 200 && response.status() < 300)
        {
            try
            {
                Wt::Json::Object tmp;
                Wt::Json::Value result;
                Wt::WString message;
                Wt::Json::parse(response.body(), result);
                tmp = result;
                message = tmp.get("alias");
                switch (mediaType)
                {
                case Enums::EMedia::email:
                    messageMailForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::sms:
                    messageSmsForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::mobileapp:
                    messagePushForTab_ += message.toUTF8() + " ";
                    break;
                }
            }
            catch (Wt::Json::ParseError const& e)
            {
                Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
            }
            catch (Wt::Json::TypeException const& e)
            {
                Wt::log("warning") << "[Alerts Widget] JSON Type Exception: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "TypeException", tr("Alert.alert.database-error"), Wt::Ok);
            }
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;

    string apiAddress = this->getApiUrl() + "/plugins/"
            + boost::lexical_cast<string>(idAll_.first.second) + "/alias"
            + "?login=" + Wt::Utils::urlEncode(session_->user()->eMail.toUTF8())
            + "&token=" + session_->user()->token.toUTF8()
            + "&user_role_id=" + boost::lexical_cast<string>(userRoleId)
            + "&media_type_id=" + boost::lexical_cast<string>(mediaType);
    Wt::Http::Client *client2 = new Wt::Http::Client(this);
    client2->done().connect(boost::bind(&AlertsWidget::getAliasPlugin, this, _1, _2, userRoleId, mediaType));
    Wt::log("debug") << "AlertsWidget : [GET] address to call : " << apiAddress;
    if (client2->get(apiAddress))
        Wt::WApplication::instance()->deferRendering();
    else
        Wt::log("error") << "Error Client Http";

}

void AlertsWidget::getAliasPlugin(boost::system::error_code err, const Wt::Http::Message& response, long long userRoleId, long long mediaType)
{
    cout << response.status() << " get Alias Plugin : " << response.body() << endl;
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() >= 200 && response.status() < 300)
        {
            try
            {
                Wt::Json::Object tmp;
                Wt::Json::Value result;
                Wt::WString message;
                Wt::Json::parse(response.body(), result);
                tmp = result;
                message = tmp.get("alias");
                switch (mediaType)
                {
                case Enums::EMedia::email:
                    messageMailForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::sms:
                    messageSmsForTab_ += message.toUTF8() + " ";
                    break;
                case Enums::EMedia::mobileapp:
                    messagePushForTab_ += message.toUTF8() + " ";
                    break;
                }
            }
            catch (Wt::Json::ParseError const& e)
            {
                Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
            }
            catch (Wt::Json::TypeException const& e)
            {
                Wt::log("warning") << "[Alerts Widget] JSON Type Exception: " << response.body();
                Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "TypeException", tr("Alert.alert.database-error"), Wt::Ok);
            }
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;
    //pour ne le faire qu'une fois ?????
    if (mediaType == 3)
    {
        fillInTabMessage();
    }
}

void AlertsWidget::deleteAlert(boost::system::error_code err, const Wt::Http::Message& response)
{
    Wt::WApplication::instance()->resumeRendering();
    if (!err)
    {
        if (response.status() != 204)
        {
            // a revoir !
            Wt::log("warning") << "[Alerts Widget] Problems parsing JSON: " << response.body();
            Wt::WMessageBox::show(tr("Alert.alert.database-error-title"), tr("Alert.alert.database-error"), Wt::Ok);
        }
    }
    else
    {
        Wt::log("error") << "[Alerts Widget] Http::Client error: " << err.message();
        Wt::WMessageBox::show(tr("Alert.alert.database-error-title") + "err", tr("Alert.alert.database-error"), Wt::Ok);
    }
    newClass_ = false;
    created_ = false;
    getResourceList();
}

