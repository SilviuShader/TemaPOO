#pragma once

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:

    UIElement();
    ~UIElement();

           void AddChild(std::shared_ptr<UIElement>);
           void Update(std::shared_ptr<UICamera>);
           void Render(std::shared_ptr<UICamera>);

    inline void SetActive(bool val)                                                { m_active = val;                        }
    inline void SetRelativePosition(DirectX::SimpleMath::Vector2 relativePosition) { m_relativePosition = relativePosition; }
    inline void SetSize(DirectX::SimpleMath::Vector2 size)                         { m_size = size;                         }

protected:

    virtual void                 ElementUpdate(std::shared_ptr<UICamera>) { }
    virtual void                 ElementRender(std::shared_ptr<UICamera>) { }
    DirectX::SimpleMath::Vector2 GetAbsolutePosition();

protected:

    DirectX::SimpleMath::Vector2 m_relativePosition;
    DirectX::SimpleMath::Vector2 m_size;
    std::shared_ptr<UIElement>   m_parent;
    bool                         m_active;

private:

    std::list<std::shared_ptr<UIElement> > m_children;
};