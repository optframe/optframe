#pragma once

#include<memory>
#include<optional>

// ===================
// Move 'Concept' here
// ===================

// Python: "don't ask for permission, ask for forgiveness"
// We cannot do that for 'undo', because if it's not 'undoable', we cannot go back (would require a copy)
// We could do this with 'apply', and return a bool (or an std::optional with disabled exception handling)
// But to prevent "simmetry" loss, it's better and simpler to keep four methods:
//
// some 'apply'
// some 'canBeApplied' ('canApply' can DEFAULT to 'return true;', while 'apply' may still fail and return 'false')
// some 'undo'
// some 'canUndo'
//
// An extra method for 'cost' is also quite welcome, as usual
//
// ==================
// maybe an arrow operator-> can bring iteroperability with older unique_ptr Move strategy (with abstract implementation)


using id_type = int;

template<class X>
using uptr = std::unique_ptr<X>;

template<class X>
using op = std::optional<X>;

//