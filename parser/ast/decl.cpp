#include "decl.h"

void TranslationUnitDecl::addDecl(std::vector<Decl*>& dls)
{
    for (int i = 0; i < dls.size(); i++) {
        if (dls[i] != nullptr) {
            decl_.push_back(dls[i]);
        }
    }
}