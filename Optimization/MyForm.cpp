#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;
[STAThread]
int main(/*array<String^>^ argv*/ int,char**)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Optimization::MyForm windowsForm;
	Application::Run(%windowsForm);
}
