/* 
 * Gui CreatePageWidget.h
 * 
 * @author ECHOES Technologies (TSA)
 * @author Guillaume KRIER
 * 
 * @date 23/09/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef CREATEPAGEWIDGET_H
#define	CREATEPAGEWIDGET_H

// Lib c++
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <string>
// Lib c
#include <stdlib.h>
// Lib Witty
#include <Wt/WValidator>
#include <Wt/WInteractWidget>
#include <Wt/WTemplateFormView>
#include <Wt/WTemplate>
#include <Wt/WDialog>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WLabel>
#include <Wt/WCssDecorationStyle>

#include <Wt/WTextArea>
#include <Wt/WTabWidget>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WStackedWidget>

#include <Wt/WInPlaceEdit>
#include <Wt/WComboBox>

/** gkr.\n
 * Max Size Name of resource
 */
#define SIZE_NAME_RESOURCE (15)

/** gkr.\n
 * vector &lsaquo; WInteractWidget* &rsaquo;
 */
typedef std::vector<Wt::WInteractWidget*> vector_type;


class CreatePageWidget : 
public Wt::WTemplateFormView
{
public:

                        CreatePageWidget(std::string namePage);
    virtual             ~CreatePageWidget();
    
    virtual void        popupAddTables(Wt::WTabWidget *tabW) {};
    virtual void        update();

    void                setButtonModif(bool check);
    void                setButtonSup(bool check);
    void                setLocalTable(bool background);
    void                setTypeButtonAdd(int check);
    void                setResourceNumberAdd(int nbResource);

protected:

    Wt::WContainerWidget                *createHeaderTable();
    Wt::WContainerWidget                *createBodyTable();
    Wt::WContainerWidget                *createFooterTable();

    virtual void                        addResource(std::vector<Wt::WInteractWidget*> argument) {created_ = false;};
    virtual void                        deleteResource(long long id) {created_= false;};
    virtual void                        modifResource(std::vector<Wt::WInteractWidget*> argument, long long id) {created_ = false;};
    virtual Wt::WValidator              *editValidator(int who) {return (new Wt::WValidator());};

    virtual std::vector<std::string>    getHeaderTableTitle() {std::vector<std::string> res; return res;};
    virtual std::vector<long long>      getIdsTable() {std::vector<long long> res; return res;};
    virtual vector_type                 getResourceRowTable(long long id) {vector_type res; return res;};

private:

    void                addInputForAffix(int rowBodyTable);
    void                showInputForAdd();
    int                checkInput(std::vector<Wt::WInteractWidget*> inputName, std::vector<Wt::WText*> errorMessage);
    void                checkAdd(std::vector<Wt::WText*> errorMessage);
    void                checkModif(long long id, std::vector<Wt::WText*> errorMessage);
    void                popupForModif(long long id);
    void                inputForModif(long long id, int rowTable, int columnTable);

    void                addResourceInHeaderTable();
    void                addButtons(long long id, int rowTable, int columnTable);  
    void                popupWindow();
    void                 popupCheck(std::vector<Wt::WInteractWidget*> inputName, std::vector<Wt::WText*> errorMessage, Wt::WDialog *dialog, long long id);
    void                fillInTable();
    void                popupComplete(Wt::WDialog *dialog);
    void                searchName(Wt::WLineEdit *arg);

    void                resourceBeAff();
    Wt::WComboBox       *getComboBox();

    std::string         nameResourcePage;
    vector_type         inputs_;
    Wt::WTable          *mediaTable_;
    bool                created_;
    bool                butModif_;
    bool                butSup_;
    bool                dial_;
    int                 butAdd_;
    int                 nbResource_;
    int                 nbAff_;
};

#endif	/* CREATEPAGEWIDGET_H */

