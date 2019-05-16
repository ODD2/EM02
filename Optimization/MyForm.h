#pragma once
#include <msclr\marshal_cppstd.h>
#include <exception>
#include <string>
#include <sstream>
#include"DataManager.h"
#include"DotNetUtilities.h"
#include "HelperFunctions.h"
#include "lex.h"
#include "FunctionDef.h"
#include "PowellMinimizer.h"
#include "SteepestMinimizer.h"
#include "NewtonMinimizer.h"
#include "QuasiNewtonMinimizer.h"
#include "ConjugateGradientMinimizer.h"
#include "Recorder.h"

namespace Optimization {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace msclr::interop;
	


	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			dataManager = new DataManager();
			//
			//TODO:  在此加入建構函式程式碼
			//
		}
	private: System::Windows::Forms::Panel^  panel1;
	public:
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadEquationsToolStripMenuItem;
	private: System::Windows::Forms::TextBox^  Input;

	private: System::Windows::Forms::Label^  InputText;
	private: System::Windows::Forms::TextBox^  Output;
	private: System::Windows::Forms::Label^  OutputText;


	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Panel^  panel3;

	private: System::Windows::Forms::TextBox^  History;

	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;

	private: System::Windows::Forms::Keys PressedKey;
	protected:
		/// <summary>
		DataManager* dataManager;
	private: System::Windows::Forms::TabControl^  tabControl1;
	protected:
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;

	private: System::Windows::Forms::TextBox^  Process;

			 FunctionDef * Function;
		/// 清除任何使用中的資源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: marshal_context  ^ context; //Converts System Variable  -> Std Variables.
	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
		/// 這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadEquationsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Input = (gcnew System::Windows::Forms::TextBox());
			this->InputText = (gcnew System::Windows::Forms::Label());
			this->Output = (gcnew System::Windows::Forms::TextBox());
			this->OutputText = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->History = (gcnew System::Windows::Forms::TextBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->Process = (gcnew System::Windows::Forms::TextBox());
			this->panel1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->AutoSize = true;
			this->panel1->Controls->Add(this->menuStrip1);
			this->panel1->Location = System::Drawing::Point(1, 2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(1312, 28);
			this->panel1->TabIndex = 0;
			// 
			// menuStrip1
			// 
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Padding = System::Windows::Forms::Padding(8, 2, 0, 2);
			this->menuStrip1->Size = System::Drawing::Size(1312, 27);
			this->menuStrip1->TabIndex = 2;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->loadEquationsToolStripMenuItem });
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(45, 23);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// loadEquationsToolStripMenuItem
			// 
			this->loadEquationsToolStripMenuItem->Name = L"loadEquationsToolStripMenuItem";
			this->loadEquationsToolStripMenuItem->Size = System::Drawing::Size(192, 26);
			this->loadEquationsToolStripMenuItem->Text = L"Load Equations";
			this->loadEquationsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadEquationsToolStripMenuItem_Click);
			// 
			// Input
			// 
			this->Input->Location = System::Drawing::Point(14, 19);
			this->Input->Multiline = true;
			this->Input->Name = L"Input";
			this->Input->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->Input->Size = System::Drawing::Size(313, 294);
			this->Input->TabIndex = 1;
			this->Input->TextChanged += gcnew System::EventHandler(this, &MyForm::Input_TextChanged);
			this->Input->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::Input_KeyDown);
			// 
			// InputText
			// 
			this->InputText->AutoSize = true;
			this->InputText->Location = System::Drawing::Point(3, 1);
			this->InputText->Name = L"InputText";
			this->InputText->Size = System::Drawing::Size(37, 15);
			this->InputText->TabIndex = 2;
			this->InputText->Text = L"Input";
			// 
			// Output
			// 
			this->Output->Location = System::Drawing::Point(14, 18);
			this->Output->Multiline = true;
			this->Output->Name = L"Output";
			this->Output->ReadOnly = true;
			this->Output->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->Output->Size = System::Drawing::Size(313, 352);
			this->Output->TabIndex = 3;
			// 
			// OutputText
			// 
			this->OutputText->AutoSize = true;
			this->OutputText->Location = System::Drawing::Point(3, 0);
			this->OutputText->Name = L"OutputText";
			this->OutputText->Size = System::Drawing::Size(62, 15);
			this->OutputText->TabIndex = 4;
			this->OutputText->Text = L"Functions";
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->InputText);
			this->panel2->Controls->Add(this->Input);
			this->panel2->Location = System::Drawing::Point(528, 36);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(334, 316);
			this->panel2->TabIndex = 5;
			// 
			// panel3
			// 
			this->panel3->Controls->Add(this->OutputText);
			this->panel3->Controls->Add(this->Output);
			this->panel3->Location = System::Drawing::Point(528, 358);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(334, 387);
			this->panel3->TabIndex = 6;
			// 
			// History
			// 
			this->History->Location = System::Drawing::Point(-4, 0);
			this->History->Multiline = true;
			this->History->Name = L"History";
			this->History->ReadOnly = true;
			this->History->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->History->Size = System::Drawing::Size(506, 688);
			this->History->TabIndex = 1;
			this->History->Text = L" ";
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(12, 36);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(510, 713);
			this->tabControl1->TabIndex = 8;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->History);
			this->tabPage1->Location = System::Drawing::Point(4, 25);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(502, 684);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"History";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->Process);
			this->tabPage2->Location = System::Drawing::Point(4, 25);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(502, 684);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Process";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// Process
			// 
			this->Process->Location = System::Drawing::Point(-4, 0);
			this->Process->Multiline = true;
			this->Process->Name = L"Process";
			this->Process->ReadOnly = true;
			this->Process->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->Process->Size = System::Drawing::Size(506, 688);
			this->Process->TabIndex = 1;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(874, 771);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"MyForm";
			this->Text = L"TEST";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
		context = gcnew marshal_context();
	}

	private: System::Void loadEquationsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		openFileDialog1->ShowDialog();
	}

    private: System::Void openFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
	//在Dialog按下OK便會進入此函式
	//字串轉制string^ to string
	std::string tempFileName;
	MarshalString(openFileDialog1->FileName, tempFileName);
	//將檔案路徑名稱傳入dataManager
	dataManager->SetFileName(tempFileName);
	//從讀取讀取向量資料
	if (dataManager->LoadEquationData())
	{
		std::vector<std::string> equations = dataManager->GetEquations();
		Output->Multiline = true;
		for (unsigned int i = 0; i < equations.size(); i++)
		{
			Output->Text += gcnew String(equations[i].c_str());
			Output->Text += Environment::NewLine;
		}
	}
}
			
    private: System::Void Input_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		if (Input->TextLength > 0 && PressedKey == System::Windows::Forms::Keys::Enter &&  Input->Text[Input->TextLength - 1] == '\n') {
			cli::array<String^> ^ deli = { "\r\n"};
			cli::array<System::String^>^ userCommand = Input->Text->Split(deli, System::StringSplitOptions::None);
			for (int i = 0; i < userCommand->Length - 1 ; ++i) {
				cli::array<System::String^>^ CmdBlock = userCommand[i]->Split(' ');
				if (CmdBlock[0] == "Func"){
					//Create input stream
					std::stringstream test(context->marshal_as<const char *>(CmdBlock[1]));

					//give input stream to flex analyser
					FlexLexer* lexer = new yyFlexLexer(&test, 0);

					delete Function;
					Function = new FunctionDef();
					//Save Original Function Form
					Function->OriginForm = context->marshal_as<const char *>(CmdBlock[1]);
					//Attach For Use.
					WorkVec = Function->GetFmu();
					VarTable = Function->GetVar();
					//Reset formula dependencies
					WorkVec->clear();
					VarTable->clear();

					//Working 
					while (lexer->yylex() != 0);
					//TODO: Error Detection.

					//Detach For Safety.
					WorkVec = NULL;
					VarTable = NULL;

					//Converting
					Infix2Postfix(*(Function->GetFmu()));

					Function->CreateTokenLink();
#ifdef OD_PRINT_PROCESS
					cout << Function->CalcFuncWithVar();
#endif
					Output->Text =gcnew String(Function->toString().c_str());
				}
				else if (CmdBlock[0] == "InitP") {
					vector<double> Values;
					//TODO:Error Detection , Dimension Insufficient
					for (int i = 1; i < CmdBlock->Length; ++i) {
						
						Values.push_back(Convert::ToDouble(CmdBlock[i]));
					}
					Function->setVariables(Values);
#ifdef OD_PRINT_PROCESS
					cout << Function->CalcFuncWithVar();
#endif
					Output->Text = gcnew String(Function->toString().c_str());
				}
				else if (CmdBlock[0] == "Limits") {
					map<string,std::pair<double,double>> Values;
					//TODO:Error Detection , Dimension Insufficient
					for (int i = 1; i < CmdBlock->Length; i+=3) {
						string var = context->marshal_as<string>(CmdBlock[i]);
						double low = Convert::ToDouble(CmdBlock[i + 1]);
						double high = Convert::ToDouble(CmdBlock[i + 2]);
						Values[var] = { low,high };
					}
					Function->setLimits(Values);
					Output->Text = gcnew String(Function->toString().c_str());
				}
				else if (CmdBlock[0] == "Powell") {
					PowellMinimizer Pm(Function);
					Vector Point =  Pm.Find();
					Output->Text += Vector2String(Point)+"\r\n";
					Output->Text += "f(X) = " + (*Function)(Point.Data).ToString() + "\r\n";
					Process->Text = gcnew String(Recorder::Record());
					Recorder::Clear();
				}
				else if (CmdBlock[0] == "Steep") {
					SteepestMinimizer Sm(Function);
					Vector Point = Sm.Find();
					Output->Text += Vector2String(Point) + "\r\n";
					Output->Text += "f(X) = " + (*Function)(Point.Data).ToString() + "\r\n";
					Process->Text = gcnew String(Recorder::Record());
					Recorder::Clear();
				}
				else if (CmdBlock[0] == "Test") {
					Output->Text += Vector2String((*Function).Gradient()) + "\r\n";
				}
				else if (CmdBlock[0] == "Newton") {
					NewtonMinimizer Nm(Function);
					Vector Point = Nm.Find();
					Output->Text += Vector2String(Point) + "\r\n";
					Output->Text += "f(X) = " + (*Function)(Point.Data).ToString() + "\r\n";
					Process->Text = gcnew String(Recorder::Record());
					Recorder::Clear();
				}
				else if (CmdBlock[0] == "QNewton") {
					QuasiNewtonMinimizer QNm(Function);
					Vector Point = QNm.Find();
					Output->Text += Vector2String(Point) + "\r\n";
					Output->Text += "f(X) = " + (*Function)(Point.Data).ToString() + "\r\n";
					Process->Text = gcnew String(Recorder::Record());
					Recorder::Clear();
				}
				else if (CmdBlock[0] == "Conjugate") {
					ConjugateGradientMinimizer CGm(Function);
					Vector Point = CGm.Find();
					Output->Text += Vector2String(Point) + "\r\n";
					Output->Text += "f(X) = " + (*Function)(Point.Data).ToString() + "\r\n";
					Process->Text = gcnew String(Recorder::Record());
					Recorder::Clear();
				}
				else {
					History->AppendText("No Such Command!!\r\n");
				}
			}
			//Record Into History.
			History->AppendText(Input->Text);
			Input->Text = "";
		}
    }
	

	private: System::String ^ Vector2String(const Vector & v) {
		String ^ ret = gcnew String("");
		ret += "[ ";
		for (int i = 0,j = v.dim(); i < j; ++i) {
			ret += v[i].ToString();
			if (i != j - 1) {
				ret += " , ";
			}
		}
		ret += " ]";
		return ret;
	}
	private: System::Void Input_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		PressedKey = e->KeyCode;
	}
};
}
