//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UserActionInitialization.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef UserActionInitialization_h
#define UserActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "constants.hh"

//------------------------------------------------------------------------------
  class UserActionInitialization : public G4VUserActionInitialization
//------------------------------------------------------------------------------
{
  public:
    UserActionInitialization();
    virtual ~UserActionInitialization();

    virtual void Build() const;
};
#endif
