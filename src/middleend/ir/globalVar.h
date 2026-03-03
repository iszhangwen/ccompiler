#pragma once

#include "usedef.h"

class GlobalVar : public User
{
public:
    enum LinkType {
        ExternType,
        InternalType,
    };

    LinkType getLinkType() const {return m_linkType;}
    void setLinkType(LinkType ty) {m_linkType = ty;}

private:

};
