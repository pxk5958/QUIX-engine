#ifndef __MenuDialog__
#define __MenuDialog__

/**
@file
Subclass of GUI_MenuDialog, which is generated by wxFormBuilder.
*/

#include "GUI_MenuDialog.h"

//// end generated include

/** Implementing GUI_MenuDialog */
class MenuDialog : public GUI_MenuDialog
{
	protected:
		// Handlers for GUI_MenuDialog events.
		void OnHost( wxCommandEvent& event );
		void OnJoin( wxCommandEvent& event );
		void OnStart( wxCommandEvent& event );
		void OnExit( wxCommandEvent& event );
	public:
		/** Constructor */
		MenuDialog( wxWindow* parent );
	//// end generated class members
	
};

#endif // __MenuDialog__
