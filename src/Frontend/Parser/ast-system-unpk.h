/* translation of file(s)
	"Frontend/Parser/formula_abstract.k"
	"Frontend/Parser/formula_rewrite.k"
	"Frontend/Parser/formula_unparse.k"
 */
/* generated by:
 *  @(#)$Author: Kimwitu++ version 2.3.13 (C) 1998-2008 Humboldt-Universitaet zu Berlin $
 */
#ifndef KC_UNPARSE_HEADER
#define KC_UNPARSE_HEADER

namespace kc { }
using namespace kc;
/* included stuff */
#line 67 "Frontend/Parser/formula_unparse.k"
#include <CoverGraph/CoverGraph.h>
#include <Formula/StatePredicate/DeadlockPredicate.h>
#include <Formula/StatePredicate/FireablePredicate.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <string>

extern std::string unparsed;

void myprinter(const char *s, kc::uview v);
void stringprinter(const char *s, kc::uview v);

#line  29 "ast-system-unpk.h"
/* end included stuff */


namespace kc {

typedef enum {
    base_uview_enum,
    out_enum,
    count_enum,
    elem_enum,
    temporal_enum,
    internal_enum,
    buechi_enum,
    ctl_enum,
    ltl_enum,
    toplevelboolean_enum,
    compound_enum,
    detectcompound_enum,
    hl_staticanalysis_enum,
    visible_enum,
    last_uview
} uview_enum;

struct impl_uviews {
    const char *name;
    uview_class *view;
};
extern impl_uviews uviews[];
class uview_class {
protected:
    // only used in derivations
    uview_class(uview_enum v): m_view(v) { }
    uview_class(c_uview): m_view(base_uview_enum)
	{ /* do not copy m_view */ }
public:
    const char* name() const
	{ return uviews[m_view].name; }
    operator uview_enum() const
	{ return m_view; }
    bool operator==(const uview_class& other) const
	{ return m_view == other.m_view; }
private:
    uview_enum m_view;
};

class printer_functor_class {
public:
    virtual void operator()(const kc_char_t*, uview) { }
    virtual ~printer_functor_class() { }
};

class printer_functor_function_wrapper : public printer_functor_class {
public:
    printer_functor_function_wrapper(const printer_function opf =0): m_old_printer(opf) { }
    virtual ~printer_functor_function_wrapper() { }
    virtual void operator()(const kc_char_t* s, uview v)
	{ if(m_old_printer) m_old_printer(s, v); }
private:
    printer_function m_old_printer;
};

/* Use uviews instead
extern char *kc_view_names[];
*/
struct base_uview_class: uview_class {
    base_uview_class():uview_class(base_uview_enum){}
};
extern base_uview_class base_uview;
struct out_class: uview_class {
    out_class():uview_class(out_enum){}
};
extern out_class out;
struct count_class: uview_class {
    count_class():uview_class(count_enum){}
};
extern count_class count;
struct elem_class: uview_class {
    elem_class():uview_class(elem_enum){}
};
extern elem_class elem;
struct temporal_class: uview_class {
    temporal_class():uview_class(temporal_enum){}
};
extern temporal_class temporal;
struct internal_class: uview_class {
    internal_class():uview_class(internal_enum){}
};
extern internal_class internal;
struct buechi_class: uview_class {
    buechi_class():uview_class(buechi_enum){}
};
extern buechi_class buechi;
struct ctl_class: uview_class {
    ctl_class():uview_class(ctl_enum){}
};
extern ctl_class ctl;
struct ltl_class: uview_class {
    ltl_class():uview_class(ltl_enum){}
};
extern ltl_class ltl;
struct toplevelboolean_class: uview_class {
    toplevelboolean_class():uview_class(toplevelboolean_enum){}
};
extern toplevelboolean_class toplevelboolean;
struct compound_class: uview_class {
    compound_class():uview_class(compound_enum){}
};
extern compound_class compound;
struct detectcompound_class: uview_class {
    detectcompound_class():uview_class(detectcompound_enum){}
};
extern detectcompound_class detectcompound;
struct hl_staticanalysis_class: uview_class {
    hl_staticanalysis_class():uview_class(hl_staticanalysis_enum){}
};
extern hl_staticanalysis_class hl_staticanalysis;
struct visible_class: uview_class {
    visible_class():uview_class(visible_enum){}
};
extern visible_class visible;

void unparse(abstract_phylum kc_p, printer_functor kc_printer, uview kc_current_view);
void unparse(void *kc_p, printer_functor kc_printer, uview kc_current_view);
void unparse(int kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(double kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(kc_char_t *kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(kc_string_t kc_v, printer_functor kc_printer, uview kc_current_view);
#define PRINT(string) kc_printer(string,kc_current_view)
#define UNPARSE(node) node->unparse(kc_printer,kc_current_view)

} // namespace kc
#endif // KC_UNPARSE_HEADER
