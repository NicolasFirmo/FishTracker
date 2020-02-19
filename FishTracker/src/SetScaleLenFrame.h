#pragma once

namespace ft {

	class FishFrame;

	class SetScaleLenFrame : public wxFrame
	{
	public:
		SetScaleLenFrame(FishFrame* parent);
		void OnSet(wxCommandEvent& evt);

	private:
		FishFrame* m_FishFrame;

		wxSpinCtrl* m_LenCtrl;
		wxChoice* m_UnityChc;
		wxButton* m_SetLenBtn;

		wxDECLARE_EVENT_TABLE();
	};

} // namespace ft