; ModuleID = 'example.2hello.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str1 = private unnamed_addr constant [5 x i8] c"%d \0A\00", align 1
@.str2 = private unnamed_addr constant [11 x i8] c"(x+1)^2 = \00", align 1
@globbbbbbbbbb = global i32 3, align 4

; Function Attrs: nounwind uwtable
define void @pop_direct_branch() #0 {
  call void asm sideeffect "popq %rbp\0A\09addq $$8, %rsp\0A\09leave\0A\09movq (%rsp), %rax\0A\09addq $$8, %rsp\0A\09jmp *%rax\0A\09", "~{dirflag},~{fpsr},~{flags}"() #2, !srcloc !1
  ret void
}

; Function Attrs: nounwind uwtable
define void @scan_int(i32* %x) #0 {
  %1 = alloca i32*, align 8
  store i32* %x, i32** %1, align 8
  %2 = load i32** %1, align 8
  %3 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32* %2)
  ret void
}

declare i32 @__isoc99_scanf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @pow2(i32 %x) #0 {
  %1 = alloca i32, align 4
  store i32 %x, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = load i32* %1, align 4
  %4 = mul nsw i32 %2, %3
  ret i32 %4
}

; Function Attrs: nounwind uwtable
define void @foo(i32* %y) #0 {
  %1 = alloca i32*, align 8
  %p = alloca i32, align 4
  store i32* %y, i32** %1, align 8
  %2 = load i32** %1, align 8
  %3 = load i32* %2, align 4
  call void @pow2_cloned(i32 %3)
  %4 = load i32* @globbbbbbbbbb, align 4
  store i32 %4, i32* %p, align 4
  %5 = load i32* %p, align 4
  %6 = load i32** %1, align 8
  %7 = load i32* %6, align 4
  %8 = mul nsw i32 %7, 2
  %9 = add nsw i32 %5, %8
  %10 = add nsw i32 %9, 1
  %11 = load i32** %1, align 8
  store i32 %10, i32* %11, align 4
  ret void
}

; Function Attrs: nounwind uwtable
define void @print_int(i32 %x) #0 {
  %1 = alloca i32, align 4
  store i32 %x, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str1, i32 0, i32 0), i32 %2)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %1
  call void @scan_int(i32* %a)
  call void @foo(i32* %a)
  %2 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0))
  %3 = load i32* %a, align 4
  call void @print_int(i32 %3)
  ret i32 0
}

; Function Attrs: nounwind uwtable
define void @pow2_cloned(i32 %x) #0 {
  %1 = alloca i32, align 4
  store i32 %x, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = load i32* %1, align 4
  %4 = mul nsw i32 %2, %3
  store i32 %4, i32* @globbbbbbbbbb, align 4
  call void @pop_direct_branch()
  ret void
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
!1 = metadata !{i32 151925, i32 151937, i32 151963, i32 151981, i32 152011, i32 152037, i32 152059}
