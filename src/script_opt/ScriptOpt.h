// See the file "COPYING" in the main distribution directory for copyright.

// Classes for controlling/orchestrating script optimization & compilation.

#pragma once

#include <optional>
#include <string>

#include "zeek/Expr.h"
#include "zeek/Func.h"
#include "zeek/Scope.h"

namespace zeek
	{
struct Options;
	}

namespace zeek::detail
	{

// Flags controlling what sorts of analysis to do.

struct AnalyOpt
	{
	// Whether to analyze scripts.
	bool activate = false;

	// Whether to optimize the AST.
	bool optimize_AST = false;

	// If true, dump out transformed code: the results of reducing
	// interpreted scripts, and, if optimize is set, of then optimizing
	// them.  Always done if only_func is set.
	bool dump_xform = false;

	// If true, dump out the use-defs for each analyzed function.
	bool dump_uds = false;

	// If non-nil, then only analyze the given function/event/hook.
	std::optional<std::string> only_func;

	// If true, do global inlining.
	bool inliner = false;

	// If true, generate C++;
	bool gen_CPP = false;

	// If true, the C++ should be standalone (not require the presence
	// of the corresponding script, and not activated by default).
	bool gen_standalone_CPP = false;

	// If true, generate C++ for those script bodies that don't already
	// have generated code, in a form that enables later compiles to
	// take advantage of the newly-added elements.  Only use for generating
	// a zeek that will always include the associated scripts.
	bool update_CPP = false;

	// If true, generate C++ for those script bodies that don't already
	// have generated code.  The added C++ is not made available for
	// later generated code, and will work for a generated zeek that
	// runs without including the associated scripts.
	bool add_CPP = false;

	// If true, use C++ bodies if available.
	bool use_CPP = false;

	// If true, compile all compileable functions, even those that
	// are inlined.  Mainly useful for ensuring compatibility for
	// some tests in the test suite.
	bool compile_all = false;

	// If true, report on available C++ bodies.
	bool report_CPP = false;

	// If true, report which functions are directly and indirectly
	// recursive, and exit.  Only germane if running the inliner.
	bool report_recursive = false;

	// If non-zero, looks for variables that are used-but-possibly-not-set,
	// or set-but-not-used.
	//
	// If > 1, also reports on uses of uninitialized record fields and
	// analyzes nested records in depth.  Warning: with the current
	// data structures this greatly increases analysis time.
	int usage_issues = 0;
	};

extern AnalyOpt analysis_options;

class ProfileFunc;

using ScriptFuncPtr = IntrusivePtr<ScriptFunc>;

// Info we need for tracking an instance of a function.
class FuncInfo
	{
public:
	FuncInfo(ScriptFuncPtr func, ScopePtr scope, StmtPtr body, int priority);

	ScriptFunc* Func() const { return func.get(); }
	const ScriptFuncPtr& FuncPtr() const { return func; }
	const ScopePtr& Scope() const { return scope; }
	const StmtPtr& Body() const { return body; }
	int Priority() const { return priority; }
	const ProfileFunc* Profile() const { return pf.get(); }
	std::shared_ptr<ProfileFunc> ProfilePtr() const { return pf; }
	const std::string& SaveFile() const { return save_file; }

	void SetBody(StmtPtr new_body) { body = std::move(new_body); }
	void SetProfile(std::shared_ptr<ProfileFunc> _pf);
	void SetSaveFile(std::string _sf) { save_file = std::move(_sf); }

	// The following provide a way of marking FuncInfo's as
	// should-be-skipped for script optimization, generally because
	// the function body has a property that a given script optimizer
	// doesn't know how to deal with.  Defaults to don't-skip.
	bool ShouldSkip() const { return skip; }
	void SetSkip(bool should_skip) { skip = should_skip; }

protected:
	ScriptFuncPtr func;
	ScopePtr scope;
	StmtPtr body;
	std::shared_ptr<ProfileFunc> pf;
	int priority;

	// Whether to skip optimizing this function.
	bool skip = false;

	// If we're saving this function in a file, this is the name
	// of the file to use.
	std::string save_file;
	};

// We track which functions are definitely not recursive.  We do this
// as the negative, rather than tracking functions known to be recursive,
// so that if we don't do the analysis at all (it's driven by inlining),
// we err on the conservative side and assume every function is recursive.
extern std::unordered_set<const Func*> non_recursive_funcs;

// Analyze a given function for optimization.
extern void analyze_func(ScriptFuncPtr f);

// Analyze the given top-level statement(s) for optimization.  Returns
// a pointer to a FuncInfo for an argument-less quasi-function that can
// be Invoked, or its body executed directly, to execute the statements.
extern const FuncInfo* analyze_global_stmts(Stmt* stmts);

// Analyze all of the parsed scripts collectively for optimization.
extern void analyze_scripts();

// Used for C++-compiled scripts to signal their presence, by setting this
// to a non-empty value.
extern void (*CPP_init_hook)();

// Used for "standalone" C++-compiled scripts to complete their activation;
// called after parsing and BiF initialization, but before zeek_init.
extern void (*CPP_activation_hook)();

	} // namespace zeek::detail
