#pragma once

#include "AssetBrowser.h"
#include "MaterialView.h"

namespace SandBoxEditor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			LoadConfigIni();
			//tabPage3->Controls->Add( gcnew MyUserControl );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected: 
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exportToGameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^  quitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  assetsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  importToolStripMenuItem;
	private: System::Windows::Forms::TreeView^  treeView1;
	private: System::Windows::Forms::ToolStripMenuItem^  setRootToolStripMenuItem;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	private: System::Windows::Forms::ToolStripMenuItem^  assetBrowserToolStripMenuItem;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton1;
	private: System::Windows::Forms::ToolStripComboBox^  toolStripComboBox1;
	private: System::Windows::Forms::ToolStripSplitButton^  toolStripSplitButton1;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::ToolStripMenuItem^  androidGLES20ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  pCDirectX11ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  configureToolStripMenuItem;
	private: System::Windows::Forms::TabControl^  tabControl2;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::TabPage^  tabPage4;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton2;
	private: System::Windows::Forms::ToolStripMenuItem^  importSDKMESHToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  directXSDKMeshsdkmeshToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->setRootToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->importSDKMESHToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->directXSDKMeshsdkmeshToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exportToGameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->quitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->assetsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->importToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->assetBrowserToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripComboBox1 = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->toolStripSplitButton1 = (gcnew System::Windows::Forms::ToolStripSplitButton());
			this->androidGLES20ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pCDirectX11ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->configureToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripButton2 = (gcnew System::Windows::Forms::ToolStripButton());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->menuStrip1->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabControl2->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->fileToolStripMenuItem, 
				this->assetsToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1129, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->setRootToolStripMenuItem, 
				this->importSDKMESHToolStripMenuItem, this->exportToGameToolStripMenuItem, this->toolStripSeparator1, this->quitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"&File";
			// 
			// setRootToolStripMenuItem
			// 
			this->setRootToolStripMenuItem->Name = L"setRootToolStripMenuItem";
			this->setRootToolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->setRootToolStripMenuItem->Text = L"Set Root";
			this->setRootToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::setRootToolStripMenuItem_Click);
			// 
			// importSDKMESHToolStripMenuItem
			// 
			this->importSDKMESHToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->directXSDKMeshsdkmeshToolStripMenuItem});
			this->importSDKMESHToolStripMenuItem->Name = L"importSDKMESHToolStripMenuItem";
			this->importSDKMESHToolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->importSDKMESHToolStripMenuItem->Text = L"Import Asset";
			// 
			// directXSDKMeshsdkmeshToolStripMenuItem
			// 
			this->directXSDKMeshsdkmeshToolStripMenuItem->Name = L"directXSDKMeshsdkmeshToolStripMenuItem";
			this->directXSDKMeshsdkmeshToolStripMenuItem->Size = System::Drawing::Size(229, 22);
			this->directXSDKMeshsdkmeshToolStripMenuItem->Text = L"DirectX SDK Mesh (.sdkmesh)";
			this->directXSDKMeshsdkmeshToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::directXSDKMeshsdkmeshToolStripMenuItem_Click);
			// 
			// exportToGameToolStripMenuItem
			// 
			this->exportToGameToolStripMenuItem->Name = L"exportToGameToolStripMenuItem";
			this->exportToGameToolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->exportToGameToolStripMenuItem->Text = L"Export to Game";
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(152, 6);
			// 
			// quitToolStripMenuItem
			// 
			this->quitToolStripMenuItem->Name = L"quitToolStripMenuItem";
			this->quitToolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->quitToolStripMenuItem->Text = L"&Quit";
			// 
			// assetsToolStripMenuItem
			// 
			this->assetsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->importToolStripMenuItem, 
				this->assetBrowserToolStripMenuItem});
			this->assetsToolStripMenuItem->Name = L"assetsToolStripMenuItem";
			this->assetsToolStripMenuItem->Size = System::Drawing::Size(52, 20);
			this->assetsToolStripMenuItem->Text = L"&Assets";
			// 
			// importToolStripMenuItem
			// 
			this->importToolStripMenuItem->Name = L"importToolStripMenuItem";
			this->importToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->importToolStripMenuItem->Text = L"&Import";
			// 
			// assetBrowserToolStripMenuItem
			// 
			this->assetBrowserToolStripMenuItem->Name = L"assetBrowserToolStripMenuItem";
			this->assetBrowserToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->assetBrowserToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->assetBrowserToolStripMenuItem->Text = L"Asset Browser";
			this->assetBrowserToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::assetBrowserToolStripMenuItem_Click);
			// 
			// treeView1
			// 
			this->treeView1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeView1->Location = System::Drawing::Point(0, 24);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(1129, 685);
			this->treeView1->TabIndex = 1;
			// 
			// folderBrowserDialog1
			// 
			this->folderBrowserDialog1->HelpRequest += gcnew System::EventHandler(this, &Form1::folderBrowserDialog1_HelpRequest);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
			this->statusStrip1->Location = System::Drawing::Point(0, 687);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(1129, 22);
			this->statusStrip1->TabIndex = 4;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(118, 17);
			this->toolStripStatusLabel1->Text = L"toolStripStatusLabel1";
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->toolStripButton1, 
				this->toolStripComboBox1, this->toolStripSplitButton1, this->toolStripButton2});
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1129, 25);
			this->toolStrip1->TabIndex = 5;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// toolStripButton1
			// 
			this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton1.Image")));
			this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton1->Name = L"toolStripButton1";
			this->toolStripButton1->Size = System::Drawing::Size(114, 22);
			this->toolStripButton1->Text = L"toolStripButton1";
			// 
			// toolStripComboBox1
			// 
			this->toolStripComboBox1->Name = L"toolStripComboBox1";
			this->toolStripComboBox1->Size = System::Drawing::Size(121, 25);
			// 
			// toolStripSplitButton1
			// 
			this->toolStripSplitButton1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->androidGLES20ToolStripMenuItem, 
				this->pCDirectX11ToolStripMenuItem, this->configureToolStripMenuItem});
			this->toolStripSplitButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripSplitButton1.Image")));
			this->toolStripSplitButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripSplitButton1->Name = L"toolStripSplitButton1";
			this->toolStripSplitButton1->Size = System::Drawing::Size(125, 22);
			this->toolStripSplitButton1->Text = L"Export to Engine";
			// 
			// androidGLES20ToolStripMenuItem
			// 
			this->androidGLES20ToolStripMenuItem->Name = L"androidGLES20ToolStripMenuItem";
			this->androidGLES20ToolStripMenuItem->Size = System::Drawing::Size(161, 22);
			this->androidGLES20ToolStripMenuItem->Text = L"Android GLES2.0";
			// 
			// pCDirectX11ToolStripMenuItem
			// 
			this->pCDirectX11ToolStripMenuItem->Name = L"pCDirectX11ToolStripMenuItem";
			this->pCDirectX11ToolStripMenuItem->Size = System::Drawing::Size(161, 22);
			this->pCDirectX11ToolStripMenuItem->Text = L"PC DirectX 11";
			// 
			// configureToolStripMenuItem
			// 
			this->configureToolStripMenuItem->Name = L"configureToolStripMenuItem";
			this->configureToolStripMenuItem->Size = System::Drawing::Size(161, 22);
			this->configureToolStripMenuItem->Text = L"Configure ...";
			// 
			// toolStripButton2
			// 
			this->toolStripButton2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton2.Image")));
			this->toolStripButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton2->Name = L"toolStripButton2";
			this->toolStripButton2->Size = System::Drawing::Size(104, 22);
			this->toolStripButton2->Text = L"Material Editor";
			this->toolStripButton2->Click += gcnew System::EventHandler(this, &Form1::toolStripButton2_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(929, 52);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(200, 632);
			this->tabControl1->TabIndex = 6;
			// 
			// tabPage1
			// 
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(192, 606);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"tabPage1";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(192, 606);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"tabPage2";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// tabControl2
			// 
			this->tabControl2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl2->Controls->Add(this->tabPage3);
			this->tabControl2->Controls->Add(this->tabPage4);
			this->tabControl2->Location = System::Drawing::Point(0, 52);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(927, 632);
			this->tabControl2->TabIndex = 7;
			// 
			// tabPage3
			// 
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(919, 606);
			this->tabPage3->TabIndex = 0;
			this->tabPage3->Text = L"tabPage3";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// tabPage4
			// 
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage4->Size = System::Drawing::Size(919, 606);
			this->tabPage4->TabIndex = 1;
			this->tabPage4->Text = L"tabPage4";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1129, 709);
			this->Controls->Add(this->tabControl2);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->treeView1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabControl2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	private: System::Void setRootToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				//folderBrowserDialog1->
			 }
	private: System::Void folderBrowserDialog1_HelpRequest(System::Object^  sender, System::EventArgs^  e) {
			 }

	public: void LoadConfigIni();
	private: System::Void assetBrowserToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				AssetBrowser ^ hdlg = gcnew AssetBrowser();
				hdlg->Show();
			}
	private: System::Void toolStripButton2_Click(System::Object^  sender, System::EventArgs^  e) {
				//MaterialBrowser ^ hDlg = gcnew MaterialBrowser();
				//hDlg->Show();

				 this->tabPage3->Controls->Add(gcnew MaterialView);
			}
	private: System::Void directXSDKMeshsdkmeshToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				if(openFileDialog1->ShowDialog()==Windows::Forms::DialogResult::OK)
				{
					// spawn a new SDKMesh importer


				}
			}
};

}
