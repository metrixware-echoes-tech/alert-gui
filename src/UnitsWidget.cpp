/* 
 * File:   UnitsWidget.cpp
 * Author: tsa
 * 
 * Created on 23 mai 2014, 16:00
 */

#include "UnitsWidget.h"

using namespace std;

UnitsWidget::UnitsWidget(Echoes::Dbo::Session *session, string apiUrl)
: AbstractPage(session, apiUrl, "unit")
{
    setButtonModif(true);
    setButtonSup(true);
    
    m_unitTypeComboBox = new Wt::WComboBox();
    m_unitTypeModel = new Wt::WStandardItemModel(0,2,this);
    
    multimap<int, string> titles;
    titles.insert(make_pair(ETypeJson::text, "name"));
    titles.insert(make_pair(ETypeJson::undid, "information_unit_type"));
    setTitles(titles);

    lists_string lListUrl;
    list_string listUrl;
    listUrl.push_back("units");
    listUrl.push_back("units/:id");
    lListUrl.push_back(listUrl);
    listUrl.clear();
    
    fillModel();
    setUrl(lListUrl);
}

UnitsWidget::~UnitsWidget()
{
}

void UnitsWidget::fillModel()
{
    m_unitTypeModel->clear();
    addEnumToModel(m_unitTypeModel, Enums::EInformationUnitType::text, tr("Alert.unit.type.text"));
    addEnumToModel(m_unitTypeModel, Enums::EInformationUnitType::number, tr("Alert.unit.type.number"));
    addEnumToModel(m_unitTypeModel, Enums::EInformationUnitType::boolean, tr("Alert.unit.type.boolean"));
    addEnumToModel(m_unitTypeModel, Enums::EInformationUnitType::custom, tr("Alert.unit.type.custom"));
}

Wt::WComboBox *UnitsWidget::popupAdd(Wt::WDialog *dialog)
{
    m_unitTypeComboBox = new Wt::WComboBox(dialog->contents());
    m_unitTypeComboBox->setModel(m_unitTypeModel);
    
    return m_unitTypeComboBox;
}

void UnitsWidget::setAddResourceMessage(Wt::Http::Message *message,vector<Wt::WInteractWidget*>* argument)
{
    vector<Wt::WInteractWidget*>::iterator it = argument->begin();
    
    message->addBodyText("{");
    message->addBodyText("\n\"name\": \"" + ((Wt::WLineEdit*)(*it++))->text().toUTF8() + "\"");
    message->addBodyText(",\n\"type_id\": " + m_unitTypeModel->item(m_unitTypeComboBox->currentIndex(), 1)->text().toUTF8());
    message->addBodyText("\n}");
}

void UnitsWidget::setModifResourceMessage(Wt::Http::Message *message,vector<Wt::WInteractWidget*>* argument)
{
    vector<Wt::WInteractWidget*>::iterator it = argument->begin();
    
    message->addBodyText("{");
    message->addBodyText("\n\"name\": \"" + ((Wt::WLineEdit*)(*it++))->text().toUTF8() + "\"");
    message->addBodyText(",\n\"type_id\": " + m_unitTypeModel->item(((Wt::WComboBox*)(*it++))->currentIndex(), 1)->text().toUTF8());
    message->addBodyText("\n}");
}

