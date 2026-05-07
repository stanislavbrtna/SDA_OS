This clang formatter options should be used.

    { 
      BasedOnStyle: LLVM, 
      UseTab: Never, 
      IndentWidth: 2,
      TabWidth: 2, 
      ColumnLimit: 100,
      SortIncludes: false,
      AlignConsecutiveMacros: Consecutive
      BinPackParameters: false,
      BinPackArguments: false,
      AlignAfterOpenBracket: Align,
      BreakBeforeBinaryOperators: None,
      AlignOperands: DontAlign,
      BreakBeforeBraces: Attach, 
      AllowShortIfStatementsOnASingleLine: false, 
      IndentCaseLabels: false, 
      AccessModifierOffset: -4, 
      NamespaceIndentation: All, 
      FixNamespaceComments: false
    }
