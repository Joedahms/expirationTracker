#ifndef ELEMENT_VISITOR_H
#define ELEMENT_VISITOR_H

class ElementVisitor {
public:
  virtual void visitCompositeElement(const CompositeElement& compositeElement) = 0;
  virtual void visitButton(const Button& button)                               = 0;
}

#endif
