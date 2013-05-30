/* 
 * File:   AlertEditionModel.h
 * Author: tsa
 *
 * Created on 3 septembre 2012, 15:01
 */

#ifndef ALERTEDITIONMODEL_H
#define	ALERTEDITIONMODEL_H

#include "GlobalIncludeFile.h"

class AlertEditionModel : public Wt::WFormModel {
public:
    AlertEditionModel(User *user);
    virtual ~AlertEditionModel();
    
    /*! \brief Enumeration for view
             */
    enum view 
    {
        Modification, 
        Creation
    };
    
    User *user;
    
    static const Field ThresholdOperator;
    static const Field ThresholdValueKey;
    static const Field ThresholdValue;
    static const Field ThresholdValueUnit;
    static const Field ThresholdValueExemple;
    static const Field Unit;
    
    virtual bool validateField(Field field);
    virtual void reset();
    
    void modifyField(Field field, const Wt::WString& info);
    void setSession(Session *session);
private:
    view currentView;
    Session *session;
    
    void setView(User *user = NULL);
    virtual bool isVisible(Field field) const;
    
    Wt::WString validateThresholdValue(Wt::WString stringToValidate) const;
    Wt::WString validateThresholdOperator(Wt::WString stringToValidate) const;
    Wt::WString validateUnit(Wt::WString stringToValidate) const;
    Wt::WString validateSnooze(Wt::WString stringToValidate) const;
    Wt::WString validateString(Wt::WString stringToValidate) const;
    
    Wt::WString label(Field field) const;
    void setValid(Field field);
};

#endif	/* ALERTEDITIONMODEL_H */

