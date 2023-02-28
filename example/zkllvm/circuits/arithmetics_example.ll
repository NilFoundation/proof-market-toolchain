; ModuleID = '/root/tmp/zkllvm/examples/arithmetics.cpp'
source_filename = "/root/tmp/zkllvm/examples/arithmetics.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }

$_ZN3nil7crypto37algebra6fields17pallas_base_field12modulus_bitsE = comdat any

$_ZN3nil7crypto37algebra6fields17pallas_base_field11number_bitsE = comdat any

$_ZN3nil7crypto37algebra6fields17pallas_base_field10value_bitsE = comdat any

$_ZN3nil7crypto37algebra6fields16vesta_base_field12modulus_bitsE = comdat any

$_ZN3nil7crypto37algebra6fields16vesta_base_field11number_bitsE = comdat any

$_ZN3nil7crypto37algebra6fields16vesta_base_field10value_bitsE = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZZN3nil7crypto314multiprecision8backends11window_bitsEmE5wsize = internal unnamed_addr constant [6 x [2 x i64]] [[2 x i64] [i64 1434, i64 7], [2 x i64] [i64 539, i64 6], [2 x i64] [i64 197, i64 4], [2 x i64] [i64 70, i64 3], [2 x i64] [i64 17, i64 2], [2 x i64] zeroinitializer], align 16
@_ZN3nil7crypto37algebra6fields17pallas_base_field12modulus_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@_ZN3nil7crypto37algebra6fields17pallas_base_field11number_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@_ZN3nil7crypto37algebra6fields17pallas_base_field10value_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@_ZN3nil7crypto37algebra6fields16vesta_base_field12modulus_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@_ZN3nil7crypto37algebra6fields16vesta_base_field11number_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@_ZN3nil7crypto37algebra6fields16vesta_base_field10value_bitsE = weak_odr dso_local local_unnamed_addr constant i64 255, comdat, align 8
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_arithmetics.cpp, ptr null }]

; Function Attrs: uwtable
define internal fastcc void @__cxx_global_var_init() unnamed_addr #0 section ".text.startup" {
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %1 = tail call i32 @__cxa_atexit(ptr @_ZNSt8ios_base4InitD1Ev, ptr @_ZStL8__ioinit, ptr @__dso_handle) #6
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: mustprogress nounwind uwtable
define dso_local noundef i64 @_ZN3nil7crypto314multiprecision8backends11window_bitsEm(i64 noundef %0) local_unnamed_addr #4 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i64 [ 5, %1 ], [ %8, %2 ]
  %4 = getelementptr inbounds [6 x [2 x i64]], ptr @_ZZN3nil7crypto314multiprecision8backends11window_bitsEmE5wsize, i64 0, i64 %3
  %5 = getelementptr inbounds [2 x i64], ptr %4, i64 0, i64 0
  %6 = load i64, ptr %5, align 16, !tbaa !5
  %7 = icmp ugt i64 %6, %0
  %8 = add i64 %3, -1
  br i1 %7, label %2, label %9, !llvm.loop !9

9:                                                ; preds = %2
  %10 = phi i64 [ %3, %2 ]
  %11 = getelementptr inbounds [6 x [2 x i64]], ptr @_ZZN3nil7crypto314multiprecision8backends11window_bitsEmE5wsize, i64 0, i64 %10
  %12 = getelementptr inbounds [2 x i64], ptr %11, i64 0, i64 1
  %13 = load i64, ptr %12, align 8, !tbaa !5
  %14 = add i64 1, %13
  ret i64 %14
}

; Function Attrs: mustprogress nounwind uwtable
define dso_local noundef __zkllvm_field_pallas_base @_Z3powu26__zkllvm_field_pallas_basei(__zkllvm_field_pallas_base noundef %0, i32 noundef %1) local_unnamed_addr #4 {
  %3 = icmp eq i32 %1, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %21

5:                                                ; preds = %2
  %6 = icmp slt i32 0, %1
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  br label %13

8:                                                ; preds = %17
  %9 = phi __zkllvm_field_pallas_base [ %15, %17 ]
  %10 = mul __zkllvm_field_pallas_base %9, %0
  br label %11

11:                                               ; preds = %8, %5
  %12 = phi __zkllvm_field_pallas_base [ %10, %8 ], [ f0x1, %5 ]
  br label %20

13:                                               ; preds = %7, %17
  %14 = phi i32 [ 0, %7 ], [ %18, %17 ]
  %15 = phi __zkllvm_field_pallas_base [ f0x1, %7 ], [ %16, %17 ]
  %16 = mul __zkllvm_field_pallas_base %15, %0
  br label %17

17:                                               ; preds = %13
  %18 = add nsw i32 %14, 1
  %19 = icmp slt i32 %18, %1
  br i1 %19, label %13, label %8, !llvm.loop !12

20:                                               ; preds = %11
  br label %21

21:                                               ; preds = %20, %4
  %22 = phi __zkllvm_field_pallas_base [ f0x1, %4 ], [ %12, %20 ]
  ret __zkllvm_field_pallas_base %22
}

; Function Attrs: circuit mustprogress nounwind uwtable
define dso_local noundef __zkllvm_field_pallas_base @_Z24field_arithmetic_exampleu26__zkllvm_field_pallas_baseu26__zkllvm_field_pallas_base(__zkllvm_field_pallas_base noundef %0, __zkllvm_field_pallas_base noundef %1) local_unnamed_addr #5 {
  %3 = add __zkllvm_field_pallas_base %0, %1
  %4 = mul __zkllvm_field_pallas_base %3, %0
  %5 = add __zkllvm_field_pallas_base %0, %1
  %6 = mul __zkllvm_field_pallas_base %1, %5
  %7 = add __zkllvm_field_pallas_base %0, %1
  %8 = mul __zkllvm_field_pallas_base %6, %7
  %9 = add __zkllvm_field_pallas_base %4, %8
  %10 = mul __zkllvm_field_pallas_base %9, %9
  %11 = mul __zkllvm_field_pallas_base %10, %9
  %12 = sub __zkllvm_field_pallas_base %1, %0
  %13 = sdiv __zkllvm_field_pallas_base %11, %12
  %14 = tail call noundef __zkllvm_field_pallas_base @_Z3powu26__zkllvm_field_pallas_basei(__zkllvm_field_pallas_base noundef %0, i32 noundef 2)
  %15 = add __zkllvm_field_pallas_base %13, %14
  %16 = add __zkllvm_field_pallas_base %15, f0x12345678901234567890
  ret __zkllvm_field_pallas_base %16
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_arithmetics.cpp() #0 section ".text.startup" {
  tail call fastcc void @__cxx_global_var_init()
  ret void
}

attributes #0 = { uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { mustprogress nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { circuit mustprogress nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 16.0.0 (git@github.com:NilFoundation/zkllvm-circifier.git 1af967026adc4c18933fa4e20db3324043912242)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"long", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = distinct !{!12, !10, !11}
