; ModuleID = 'MagicFunc.cpp'
source_filename = "MagicFunc.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = dso_local local_unnamed_addr global [100 x [100 x i32]] zeroinitializer, align 16
@b = dso_local local_unnamed_addr global [100 x [100 x i32]] zeroinitializer, align 16
@c = dso_local local_unnamed_addr global [100 x [100 x i32]] zeroinitializer, align 16

; Function Attrs: mustprogress nofree norecurse nosync nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #0 {
  br label %1

1:                                                ; preds = %0, %53
  %2 = phi i64 [ 0, %0 ], [ %6, %53 ]
  %3 = add nsw i64 %2, -1
  %4 = icmp eq i64 %2, 0
  %5 = xor i1 %4, true
  %6 = add nuw nsw i64 %2, 1
  %7 = icmp ult i64 %2, 99
  %8 = xor i1 %7, true
  br i1 %4, label %9, label %55

9:                                                ; preds = %1, %49
  %10 = phi i64 [ %50, %49 ], [ 0, %1 ]
  %11 = icmp ne i64 %10, 99
  %12 = select i1 %5, i1 %11, i1 false
  br i1 %12, label %13, label %23

13:                                               ; preds = %9
  %14 = add nuw nsw i64 %10, 1
  %15 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %3, i64 %10
  %16 = load i32, i32* %15, align 4, !tbaa !5
  %17 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 0, i64 %14
  %18 = load i32, i32* %17, align 4, !tbaa !5
  %19 = mul nsw i32 %18, %16
  %20 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 0, i64 %10
  %21 = load i32, i32* %20, align 4, !tbaa !5
  %22 = add nsw i32 %21, %19
  store i32 %22, i32* %20, align 4, !tbaa !5
  br label %23

23:                                               ; preds = %13, %9
  %24 = select i1 %7, i1 %11, i1 false
  br i1 %24, label %25, label %35

25:                                               ; preds = %23
  %26 = add nuw nsw i64 %10, 1
  %27 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 0, i64 %26
  %28 = load i32, i32* %27, align 4, !tbaa !5
  %29 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 %6, i64 %10
  %30 = load i32, i32* %29, align 4, !tbaa !5
  %31 = mul nsw i32 %30, %28
  %32 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 0, i64 %10
  %33 = load i32, i32* %32, align 4, !tbaa !5
  %34 = add nsw i32 %33, %31
  store i32 %34, i32* %32, align 4, !tbaa !5
  br label %35

35:                                               ; preds = %25, %23
  %36 = icmp eq i64 %10, 0
  %37 = select i1 %8, i1 true, i1 %36
  br i1 %37, label %49, label %38

38:                                               ; preds = %35
  %39 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %6, i64 %10
  %40 = load i32, i32* %39, align 4, !tbaa !5
  %41 = add nuw i64 %10, 4294967295
  %42 = and i64 %41, 4294967295
  %43 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 0, i64 %42
  %44 = load i32, i32* %43, align 4, !tbaa !5
  %45 = mul nsw i32 %44, %40
  %46 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 0, i64 %10
  %47 = load i32, i32* %46, align 4, !tbaa !5
  %48 = add nsw i32 %47, %45
  store i32 %48, i32* %46, align 4, !tbaa !5
  br label %49

49:                                               ; preds = %38, %35
  %50 = add nuw nsw i64 %10, 1
  %51 = icmp eq i64 %50, 100
  br i1 %51, label %53, label %9, !llvm.loop !9

52:                                               ; preds = %53
  ret i32 0

53:                                               ; preds = %99, %49
  %54 = icmp eq i64 %6, 100
  br i1 %54, label %52, label %1, !llvm.loop !12

55:                                               ; preds = %1, %99
  %56 = phi i64 [ %109, %99 ], [ 0, %1 ]
  %57 = icmp ne i64 %56, 99
  %58 = select i1 %5, i1 %57, i1 false
  br i1 %58, label %59, label %69

59:                                               ; preds = %55
  %60 = add nuw nsw i64 %56, 1
  %61 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %3, i64 %56
  %62 = load i32, i32* %61, align 4, !tbaa !5
  %63 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 %2, i64 %60
  %64 = load i32, i32* %63, align 4, !tbaa !5
  %65 = mul nsw i32 %64, %62
  %66 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 %2, i64 %56
  %67 = load i32, i32* %66, align 4, !tbaa !5
  %68 = add nsw i32 %67, %65
  store i32 %68, i32* %66, align 4, !tbaa !5
  br label %69

69:                                               ; preds = %59, %55
  %70 = select i1 %7, i1 %57, i1 false
  br i1 %70, label %71, label %81

71:                                               ; preds = %69
  %72 = add nuw nsw i64 %56, 1
  %73 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %2, i64 %72
  %74 = load i32, i32* %73, align 4, !tbaa !5
  %75 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 %6, i64 %56
  %76 = load i32, i32* %75, align 4, !tbaa !5
  %77 = mul nsw i32 %76, %74
  %78 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 %2, i64 %56
  %79 = load i32, i32* %78, align 4, !tbaa !5
  %80 = add nsw i32 %79, %77
  store i32 %80, i32* %78, align 4, !tbaa !5
  br label %81

81:                                               ; preds = %71, %69
  %82 = icmp eq i64 %56, 0
  %83 = select i1 %8, i1 true, i1 %82
  br i1 %83, label %84, label %88

84:                                               ; preds = %81
  %85 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 %2, i64 %56
  %86 = load i32, i32* %85, align 4, !tbaa !5
  %87 = add nsw i64 %56, -1
  br label %99

88:                                               ; preds = %81
  %89 = add nsw i64 %56, -1
  %90 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %6, i64 %56
  %91 = load i32, i32* %90, align 4, !tbaa !5
  %92 = and i64 %89, 4294967295
  %93 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 %2, i64 %92
  %94 = load i32, i32* %93, align 4, !tbaa !5
  %95 = mul nsw i32 %94, %91
  %96 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 %2, i64 %56
  %97 = load i32, i32* %96, align 4, !tbaa !5
  %98 = add nsw i32 %97, %95
  br label %99

99:                                               ; preds = %84, %88
  %100 = phi i64 [ %87, %84 ], [ %89, %88 ]
  %101 = phi i32 [ %86, %84 ], [ %98, %88 ]
  %102 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %2, i64 %100
  %103 = load i32, i32* %102, align 4, !tbaa !5
  %104 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @b, i64 0, i64 %3, i64 %56
  %105 = load i32, i32* %104, align 4, !tbaa !5
  %106 = mul nsw i32 %105, %103
  %107 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @c, i64 0, i64 %2, i64 %56
  %108 = add nsw i32 %101, %106
  store i32 %108, i32* %107, align 4, !tbaa !5
  %109 = add nuw nsw i64 %56, 1
  %110 = icmp eq i64 %109, 100
  br i1 %110, label %53, label %55, !llvm.loop !9
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = distinct !{!12, !10, !11}
