; ModuleID = 'parallel-add.c'
source_filename = "parallel-add.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%union.pthread_attr_t = type { i64, [48 x i8] }

@n = dso_local constant i32 10000, align 4
@m = dso_local constant i32 10, align 4
@a = dso_local global [10000 x i32] zeroinitializer, align 16
@b = dso_local global [10000 x i32] zeroinitializer, align 16
@c = dso_local global [10000 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @sum(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i8* %0, i8** %3, align 8
  %7 = load i8*, i8** %3, align 8
  %8 = bitcast i8* %7 to i32*
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %4, align 4
  %10 = load i8*, i8** %3, align 8
  %11 = bitcast i8* %10 to i32*
  %12 = getelementptr inbounds i32, i32* %11, i64 1
  %13 = load i32, i32* %12, align 4
  store i32 %13, i32* %5, align 4
  %14 = load i32, i32* %4, align 4
  store i32 %14, i32* %6, align 4
  br label %15

15:                                               ; preds = %32, %1
  %16 = load i32, i32* %6, align 4
  %17 = load i32, i32* %5, align 4
  %18 = icmp slt i32 %16, %17
  br i1 %18, label %19, label %35

19:                                               ; preds = %15
  %20 = load i32, i32* %6, align 4
  %21 = sext i32 %20 to i64
  %22 = getelementptr inbounds [10000 x i32], [10000 x i32]* @a, i64 0, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = load i32, i32* %6, align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr inbounds [10000 x i32], [10000 x i32]* @b, i64 0, i64 %25
  %27 = load i32, i32* %26, align 4
  %28 = add nsw i32 %23, %27
  %29 = load i32, i32* %6, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds [10000 x i32], [10000 x i32]* @c, i64 0, i64 %30
  store i32 %28, i32* %31, align 4
  br label %32

32:                                               ; preds = %19
  %33 = load i32, i32* %6, align 4
  %34 = add nsw i32 %33, 1
  store i32 %34, i32* %6, align 4
  br label %15, !llvm.loop !6

35:                                               ; preds = %15
  %36 = load i8*, i8** %2, align 8
  ret i8* %36
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  br label %8

8:                                                ; preds = %21, %0
  %9 = load i32, i32* %2, align 4
  %10 = icmp slt i32 %9, 10000
  br i1 %10, label %11, label %24

11:                                               ; preds = %8
  %12 = load i32, i32* %2, align 4
  %13 = load i32, i32* %2, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds [10000 x i32], [10000 x i32]* @a, i64 0, i64 %14
  store i32 %12, i32* %15, align 4
  %16 = load i32, i32* %2, align 4
  %17 = sub nsw i32 10000, %16
  %18 = load i32, i32* %2, align 4
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds [10000 x i32], [10000 x i32]* @b, i64 0, i64 %19
  store i32 %17, i32* %20, align 4
  br label %21

21:                                               ; preds = %11
  %22 = load i32, i32* %2, align 4
  %23 = add nsw i32 %22, 1
  store i32 %23, i32* %2, align 4
  br label %8, !llvm.loop !8

24:                                               ; preds = %8
  %25 = call i8* @llvm.stacksave()
  store i8* %25, i8** %3, align 8
  %26 = alloca i64, i64 10, align 16
  %27 = alloca [2 x i32], i64 10, align 16
  store i32 0, i32* %4, align 4
  br label %28

28:                                               ; preds = %56, %24
  %29 = load i32, i32* %4, align 4
  %30 = icmp slt i32 %29, 10
  br i1 %30, label %31, label %59

31:                                               ; preds = %28
  %32 = load i32, i32* %4, align 4
  %33 = mul nsw i32 %32, 10000
  %34 = sdiv i32 %33, 10
  %35 = load i32, i32* %4, align 4
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds [2 x i32], [2 x i32]* %27, i64 %36
  %38 = getelementptr inbounds [2 x i32], [2 x i32]* %37, i64 0, i64 0
  store i32 %34, i32* %38, align 8
  %39 = load i32, i32* %4, align 4
  %40 = add nsw i32 %39, 1
  %41 = mul nsw i32 %40, 10000
  %42 = sdiv i32 %41, 10
  %43 = load i32, i32* %4, align 4
  %44 = sext i32 %43 to i64
  %45 = getelementptr inbounds [2 x i32], [2 x i32]* %27, i64 %44
  %46 = getelementptr inbounds [2 x i32], [2 x i32]* %45, i64 0, i64 1
  store i32 %42, i32* %46, align 4
  %47 = load i32, i32* %4, align 4
  %48 = sext i32 %47 to i64
  %49 = getelementptr inbounds i64, i64* %26, i64 %48
  %50 = load i32, i32* %4, align 4
  %51 = sext i32 %50 to i64
  %52 = getelementptr inbounds [2 x i32], [2 x i32]* %27, i64 %51
  %53 = getelementptr inbounds [2 x i32], [2 x i32]* %52, i64 0, i64 0
  %54 = bitcast i32* %53 to i8*
  %55 = call i32 @pthread_create(i64* noundef %49, %union.pthread_attr_t* noundef null, i8* (i8*)* noundef @sum, i8* noundef %54) #4
  br label %56

56:                                               ; preds = %31
  %57 = load i32, i32* %4, align 4
  %58 = add nsw i32 %57, 1
  store i32 %58, i32* %4, align 4
  br label %28, !llvm.loop !9

59:                                               ; preds = %28
  store i32 0, i32* %5, align 4
  br label %60

60:                                               ; preds = %69, %59
  %61 = load i32, i32* %5, align 4
  %62 = icmp slt i32 %61, 10
  br i1 %62, label %63, label %72

63:                                               ; preds = %60
  %64 = load i32, i32* %5, align 4
  %65 = sext i32 %64 to i64
  %66 = getelementptr inbounds i64, i64* %26, i64 %65
  %67 = load i64, i64* %66, align 8
  %68 = call i32 @pthread_join(i64 noundef %67, i8** noundef null)
  br label %69

69:                                               ; preds = %63
  %70 = load i32, i32* %5, align 4
  %71 = add nsw i32 %70, 1
  store i32 %71, i32* %5, align 4
  br label %60, !llvm.loop !10

72:                                               ; preds = %60
  store i32 0, i32* %6, align 4
  br label %73

73:                                               ; preds = %84, %72
  %74 = load i32, i32* %6, align 4
  %75 = icmp slt i32 %74, 10000
  br i1 %75, label %76, label %87

76:                                               ; preds = %73
  %77 = load i32, i32* %6, align 4
  %78 = sext i32 %77 to i64
  %79 = getelementptr inbounds [10000 x i32], [10000 x i32]* @c, i64 0, i64 %78
  %80 = load i32, i32* %79, align 4
  %81 = icmp ne i32 %80, 10000
  br i1 %81, label %82, label %83

82:                                               ; preds = %76
  store i32 -1, i32* %1, align 4
  store i32 1, i32* %7, align 4
  br label %88

83:                                               ; preds = %76
  br label %84

84:                                               ; preds = %83
  %85 = load i32, i32* %6, align 4
  %86 = add nsw i32 %85, 1
  store i32 %86, i32* %6, align 4
  br label %73, !llvm.loop !11

87:                                               ; preds = %73
  store i32 0, i32* %1, align 4
  store i32 1, i32* %7, align 4
  br label %88

88:                                               ; preds = %87, %82
  %89 = load i8*, i8** %3, align 8
  call void @llvm.stackrestore(i8* %89)
  %90 = load i32, i32* %1, align 4
  ret i32 %90
}

; Function Attrs: nofree nosync nounwind willreturn
declare i8* @llvm.stacksave() #1

; Function Attrs: nounwind
declare i32 @pthread_create(i64* noundef, %union.pthread_attr_t* noundef, i8* (i8*)* noundef, i8* noundef) #2

declare i32 @pthread_join(i64 noundef, i8** noundef) #3

; Function Attrs: nofree nosync nounwind willreturn
declare void @llvm.stackrestore(i8*) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind willreturn }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
