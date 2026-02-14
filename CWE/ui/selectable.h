#pragma once
#include "../SA2ModLoader.h"
#include <cstdint>
#include <string>
#include <memory>

enum class Direction {
	Left,
	Right,
	Up,
	Down
};

class UIController;
class UISelectable {

private:
	uint32_t m_flags;
	bool m_selected = false;
public:
	std::string m_name;
	enum SelectFlag : uint32_t {
		CantSelect = BIT_0,
		ForceHorizontal = BIT_1,
		ForceVertical = BIT_2,
	};
	//please clean this up later, quick hack for finishing update
	std::shared_ptr<UISelectable> m_selectForceRight = nullptr;

	//these arent necessary to init, because they are only used for deciding what to select
	float m_posX = 0, m_posY = 0;
	float GetAspectRatio() {
		return (HorizontalResolution / VerticalResolution);
	}
	bool IsForcedHorizontal() {
		return (m_flags & ForceHorizontal);
	}
	bool IsForcedVertical() {
		return (m_flags & ForceVertical);
	}
	bool IsSelectable() {
		return !(m_flags & CantSelect);
	}
	void SetSelectable(bool select) {
		if (select) m_flags &= ~CantSelect;
		else m_flags |= CantSelect;
	}
	void Select() {
		m_selected = true;
	}
	void Unselect() {
		m_selected = false;
	}
	bool IsSelected() const { 
		return m_selected; 
	}
	virtual bool CanUnselect(Direction direction) const { return true; }
	virtual void Press(UIController* controller) {}
	virtual void Exec() {}
	virtual void Disp() {}
	UISelectable(const std::string& name, uint32_t flags = 0) : m_name(name), m_flags(flags) {

	}
};
