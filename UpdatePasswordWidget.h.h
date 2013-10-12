// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_AUTH_UPDATE_PASSWORD_WIDGET_H_
#define WT_AUTH_UPDATE_PASSWORD_WIDGET_H_

#include <Wt/WTemplateFormView>
#include <Wt/Auth/RegistrationModel>

namespace Wt {
  namespace Auth {

class AuthModel;
class RegistrationModel;

/*! \class UpdatePasswordWidget Auth/UpdatePasswordWidget
 *  \brief A widget which allows a user to choose a new password.
 *
 * This widget lets a user choose a new password.
 *
 * The widget renders the <tt>"Wt.Auth.template.update-password"</tt>
 * template. Optionally, it asks for the current password, as well as
 * a new password.
 *
 * \sa AuthWidget::createUpdatePasswordView()
 *
 * \ingroup auth
 */
class WT_API UpdatePasswordWidget : public WTemplateFormView
{
public:
  /*! \brief Constructor.
   *
   * If \p authModel is not \c null, the user also has to authenticate
   * first using his current password.
   */
  UpdatePasswordWidget(const User& user, RegistrationModel *registrationModel,
		       AuthModel *authModel, WContainerWidget *parent = 0);

protected:
  virtual WFormWidget *createFormWidget(WFormModel::Field field);

private:
  User user_;

  RegistrationModel *registrationModel_;
  AuthModel *authModel_;

  void checkPassword();
  void checkPassword2();
  bool validate();
  void doUpdate();
  void close();
};

  }
}

#endif // WT_AUTH_UPDATE_PASSWORD_WIDGET_H_
