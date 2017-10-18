#pragma once

namespace SandBoxEditor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for AssetBrowser
	/// </summary>
	public ref class AssetBrowser : public System::Windows::Forms::Form
	{
	public:
		AssetBrowser(void)
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
		~AssetBrowser()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TreeView^  treeView1;
	protected: 
	private: System::Windows::Forms::ListView^  listView1;

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
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->listView1 = (gcnew System::Windows::Forms::ListView());
			this->SuspendLayout();
			// 
			// treeView1
			// 
			this->treeView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->treeView1->Location = System::Drawing::Point(12, 12);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(161, 368);
			this->treeView1->TabIndex = 0;
			// 
			// listView1
			// 
			this->listView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listView1->Location = System::Drawing::Point(180, 13);
			this->listView1->Name = L"listView1";
			this->listView1->Size = System::Drawing::Size(498, 367);
			this->listView1->TabIndex = 1;
			this->listView1->UseCompatibleStateImageBehavior = false;
			// 
			// AssetBrowser
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(690, 392);
			this->Controls->Add(this->listView1);
			this->Controls->Add(this->treeView1);
			this->Name = L"AssetBrowser";
			this->Text = L"AssetBrowser";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
