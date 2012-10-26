/* 
 * File:   RegistrationWidget.h
 * Author: tsa
 *
 * Created on 14 août 2012, 11:50
 */

#ifndef ALERTEDITIONWIDGET_H
#define	ALERTEDITIONWIDGET_H

#include "GlobalIncludeFile.h"

#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WGroupBox>
#include <Wt/WBreak>


#include "Wt/WAnchor"
#include "Wt/WApplication"
#include "Wt/WContainerWidget"
#include "Wt/WDialog"
#include "Wt/WImage"
#include "Wt/WLineEdit"
#include "Wt/WPushButton"
#include "Wt/WText"

#include <Wt/WComboBox>
#include <Wt/WSelectionBox>
#include <Wt/WStringListModel>
#include <Wt/Dbo/Query>
#include <Wt/WSpinBox>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WMessageBox>



#include <memory>
#include <Wt/WTableView>

class EchoesHome;
class UserActionManagement;

class AlertEditionWidget : public Wt::WTemplateFormView
{
public:
  /*! \brief Constructor
   *
   * Creates a new authentication.
   */
  AlertEditionWidget();

  /*! \brief Sets the registration model.
   */
  void setModel(AlertEditionModel *model);
  
  /*! \brief Sets the session.
   */
  void setSession(Session *session);

  /*! \brief Returns the registration model.
   *
   * This returns the model that is used by the widget to do the actual
   * registration.
   */
  AlertEditionModel *model() const { return model_; }
  
  void checkThresholdOperator();
  void checkThresholdValue();
  void checkSnoozeValue();
  
  /*! \brief Updates the user-interface.
   *
   * This updates the user-interface to reflect the current state of the
   * model.
   */
  void update();
  
  long long userId;
  
  // maps needed to keep the link to the base
  std::map <int,long long> mapAssetIdSboxRow;
  Wt::WSelectionBox *serverSelectionBox;
  std::map<int,long long> mapPluginIdSboxRow;
  Wt::WSelectionBox *applicationSelectionBox;
  std::map<int,InformationId> mapInformationIdSboxRow;
  Wt::WSelectionBox *informationSelectionBox;
  
  // list of media where the alerts are sent
  Wt::WTableView *userMediaDestinationTableView;
  std::map<int,long long> mapAlertMediaSpecializationIdTableView;
  
  Wt::WComboBox *comboAlertCrit;
  
  std::map<int,long long> mapUserIdSboxRow;
  Wt::WSelectionBox *userSelectionBox;
  std::map<int,long long> mapMediaIdSboxRow;
  Wt::WSelectionBox *mediaSelectionBox;
  std::map<int,long long> mapMediaValueIdSboxRow;
  Wt::WSelectionBox *mediaValueSelectionBox;
  
  std::map<int,long long> mapAlertCriteriaIdSboxRow;
  
  void updateMediaSBFromUserSB();
  void updateMediaValueSBFromMediaSB();
  
  void updateApplicationSBFromServerSB();
  void updateInformationSBFromApplicationSB();
  
  void updateMediaValueSelectionBox(int userId, int medId);
  void updateMediaSelectionBox(int userId);
  void updateUserSelectionBox(int userId);
  
  void updateServerSelectionBox(int serverId);
  void updateInformationSelectionBox(int pluginId);
  void updateApplicationSelectionBox(int astId);
  
  Wt::WStringListModel *getMediasForCurrentUser(int mediaType);
  
  
  void addMedia();
  void deleteMedia();
  
  void addAlert();


protected:
  /*! \brief Validates the current information.
   *
   * The default implementation simply calls
   * RegistrationModel::validate() on the model.
   *
   * You may want to reimplement this method if you've added other
   * information to the registration form that need validation.
   */
  virtual bool validate();


  /*! \brief Closes the registration widget.
   *
   * The default implementation simply deletes the widget.
   */
  virtual void close();


  virtual void render(Wt::WFlags<Wt::RenderFlag> flags);

protected:
  virtual Wt::WFormWidget *createFormWidget(AlertEditionModel::Field field);

private:
  AlertEditionModel *model_;
  Session * session;

  bool created_;
  
  Wt::WGroupBox *organizationContainer;
  Wt::WButtonGroup *organizationGroup;

};



#endif	/* ALERTEDITIONWIDGET_H */

