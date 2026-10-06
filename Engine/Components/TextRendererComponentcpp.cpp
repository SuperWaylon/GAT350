#include "pch.h"
#include "TextRendererComponent.h"
#include "Framework/Actor.h"
#include "Renderer/Renderer.h"
#include "Renderer/TextRenderer.h"
#include "Core/Factory.h"
#include "Resources/ResourceManager.h"

namespace nu
{
	FACTORY_REGISTER(TextRendererComponent)

	TextRendererComponent::TextRendererComponent(const TextRendererComponent& other)
	{
		m_textString = other.m_textString;
		m_fontName = other.m_fontName;
		m_fontSize = other.m_fontSize;
		m_color = other.m_color;

		m_textChanged = true;
		if (other.m_text)
		{
			m_text = std::make_unique<TextRenderer>(*other.m_text.get());
		}
	}

	void TextRendererComponent::Update(float dt)
	{
		if (!m_text && !m_fontName.empty())
		{
			res_t<Font> font = ResourceManager::Instance().Get<Font>(m_fontName, m_fontSize);
			m_text = std::make_unique<TextRenderer>(font);
		}
	}

	void TextRendererComponent::Draw(const Renderer& renderer)
	{
		// update text if text has changed
		if (m_textChanged)
		{
			m_textChanged = false;
			m_text->Create(renderer, m_textString, m_color);
		}

		//renderer.DrawTexture(m_text->GetTexture(), GetOwner()->GetTransform());
	}

	void TextRendererComponent::SetText(const std::string& textString)
	{
		// check for new text
		if (m_textString != textString)
		{
			m_textString = textString;
			m_textChanged = true;
		}
	}

	void TextRendererComponent::Read(const json::value_t& value)
	{
		JSON_READ_NAME(value, "text", m_textString);
		JSON_READ_NAME(value, "font_name", m_fontName);
		JSON_READ_NAME(value, "font_size", m_fontSize);
		JSON_READ_NAME(value, "color", m_color);
	}

}