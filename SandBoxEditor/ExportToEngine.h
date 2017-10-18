#pragma once

namespace SandBoxEditor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ExportToEngine
	/// </summary>
	public ref class ExportToEngine : public System::Windows::Forms::Form
	{
	public:
		ExportToEngine(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ExportToEngine()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Button^  m_BrowseButton;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::CheckBox^  m_ExportAndroidTegra;
	private: System::Windows::Forms::CheckBox^  m_ExportDX11;

	protected: 







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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->m_BrowseButton = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->m_ExportAndroidTegra = (gcnew System::Windows::Forms::CheckBox());
			this->m_ExportDX11 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->progressBar1);
			this->groupBox1->Controls->Add(this->m_BrowseButton);
			this->groupBox1->Controls->Add(this->textBox1);
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Controls->Add(this->m_ExportAndroidTegra);
			this->groupBox1->Controls->Add(this->m_ExportDX11);
			this->groupBox1->Location = System::Drawing::Point(12, 10);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(786, 70);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Export Runtime datas";
			// 
			// progressBar1
			// 
			this->progressBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->progressBar1->Location = System::Drawing::Point(210, 41);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(570, 23);
			this->progressBar1->TabIndex = 5;
			// 
			// m_BrowseButton
			// 
			this->m_BrowseButton->Location = System::Drawing::Point(210, 17);
			this->m_BrowseButton->Name = L"m_BrowseButton";
			this->m_BrowseButton->Size = System::Drawing::Size(65, 23);
			this->m_BrowseButton->TabIndex = 4;
			this->m_BrowseButton->Text = L"Browse";
			this->m_BrowseButton->UseVisualStyleBackColor = true;
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Location = System::Drawing::Point(281, 18);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(499, 20);
			this->textBox1->TabIndex = 3;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(106, 17);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(97, 43);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Export";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// m_ExportAndroidTegra
			// 
			this->m_ExportAndroidTegra->AutoSize = true;
			this->m_ExportAndroidTegra->Location = System::Drawing::Point(7, 43);
			this->m_ExportAndroidTegra->Name = L"m_ExportAndroidTegra";
			this->m_ExportAndroidTegra->Size = System::Drawing::Size(93, 17);
			this->m_ExportAndroidTegra->TabIndex = 1;
			this->m_ExportAndroidTegra->Text = L"Android Tegra";
			this->m_ExportAndroidTegra->UseVisualStyleBackColor = true;
			// 
			// m_ExportDX11
			// 
			this->m_ExportDX11->AutoSize = true;
			this->m_ExportDX11->Checked = true;
			this->m_ExportDX11->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_ExportDX11->Location = System::Drawing::Point(7, 20);
			this->m_ExportDX11->Name = L"m_ExportDX11";
			this->m_ExportDX11->Size = System::Drawing::Size(76, 17);
			this->m_ExportDX11->TabIndex = 0;
			this->m_ExportDX11->Text = L"DirectX 11";
			this->m_ExportDX11->UseVisualStyleBackColor = true;
			// 
			// ExportToEngine
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(810, 92);
			this->Controls->Add(this->groupBox1);
			this->Name = L"ExportToEngine";
			this->Text = L"ExportToEngine";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
