#pragma once
#include "RendererComponent.h"
#include "Math/Vector3.h"

namespace nu
{
	class TextRendererComponent : public RendererComponent
	{
	public:
		TextRendererComponent() = default;
		TextRendererComponent(const TextRendererComponent& other);

		CLASS_PROTOTYPE(TextRendererComponent)

		void Update(float dt) override;
		void Draw(const class Renderer& renderer) override;

		void SetText(const std::string& textString);
		const std::string& GetText() const { return m_textString; }

		void Read(const json::value_t& value);

	protected:
		std::string m_textString;
		bool m_textChanged = true;

		std::string m_fontName;
		float m_fontSize = 8.0f;
		Color m_color{ 1.0f, 1.0f, 1.0f };

		std::unique_ptr<class TextRenderer> m_text;
	};
}