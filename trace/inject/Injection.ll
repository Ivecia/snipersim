; ModuleID = 'Injection.c'
source_filename = "Injection.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %struct._IO_codecvt*, %struct._IO_wide_data*, %struct._IO_FILE*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type opaque
%struct._IO_codecvt = type opaque
%struct._IO_wide_data = type opaque
%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }

@trace_file = dso_local thread_local global %struct._IO_FILE* null, align 8
@main_thread_id = internal global i64 0, align 8
@.str = private unnamed_addr constant [15 x i8] c"trace_main.txt\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"w\00", align 1
@trace_id_mutex = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@trace_file_created = dso_local thread_local global i8 0, align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"trace_%d.txt\00", align 1
@trace_id = internal global i32 0, align 4
@.str.3 = private unnamed_addr constant [5 x i8] c"%lu\0A\00", align 1
@.str.4 = private unnamed_addr constant [3 x i8] c"1\0A\00", align 1
@.str.5 = private unnamed_addr constant [3 x i8] c"0\0A\00", align 1
@fd_va = internal global i32 -1, align 4
@.str.6 = private unnamed_addr constant [19 x i8] c"/proc/self/pagemap\00", align 1
@.str.7 = private unnamed_addr constant [12 x i8] c"fd_va != -1\00", align 1
@.str.8 = private unnamed_addr constant [12 x i8] c"Injection.c\00", align 1
@__PRETTY_FUNCTION__.va2pa = private unnamed_addr constant [25 x i8] c"uint64_t va2pa(uint64_t)\00", align 1
@.str.9 = private unnamed_addr constant [16 x i8] c"offset == index\00", align 1
@.str.10 = private unnamed_addr constant [9 x i8] c"pfn != 0\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @registerMainThreadID() #0 {
  %1 = call i64 @pthread_self() #5
  store i64 %1, i64* @main_thread_id, align 8
  %2 = call noalias %struct._IO_FILE* @fopen(i8* noundef getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i64 0, i64 0), i8* noundef getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0))
  store %struct._IO_FILE* %2, %struct._IO_FILE** @trace_file, align 8
  ret void
}

; Function Attrs: nounwind readnone willreturn
declare i64 @pthread_self() #1

declare noalias %struct._IO_FILE* @fopen(i8* noundef, i8* noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @enterFunction() #0 {
  %1 = alloca i64, align 8
  %2 = alloca [20 x i8], align 16
  %3 = call i64 @pthread_self() #5
  store i64 %3, i64* %1, align 8
  %4 = load i64, i64* %1, align 8
  %5 = load i64, i64* @main_thread_id, align 8
  %6 = icmp eq i64 %4, %5
  br i1 %6, label %7, label %8

7:                                                ; preds = %0
  br label %21

8:                                                ; preds = %0
  %9 = load i8, i8* @trace_file_created, align 1
  %10 = trunc i8 %9 to i1
  br i1 %10, label %21, label %11

11:                                               ; preds = %8
  store i8 1, i8* @trace_file_created, align 1
  %12 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* noundef @trace_id_mutex) #6
  %13 = getelementptr inbounds [20 x i8], [20 x i8]* %2, i64 0, i64 0
  %14 = load i32, i32* @trace_id, align 4
  %15 = call i32 (i8*, i8*, ...) @sprintf(i8* noundef %13, i8* noundef getelementptr inbounds ([13 x i8], [13 x i8]* @.str.2, i64 0, i64 0), i32 noundef %14) #6
  %16 = load i32, i32* @trace_id, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* @trace_id, align 4
  %18 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* noundef @trace_id_mutex) #6
  %19 = getelementptr inbounds [20 x i8], [20 x i8]* %2, i64 0, i64 0
  %20 = call noalias %struct._IO_FILE* @fopen(i8* noundef %19, i8* noundef getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0))
  store %struct._IO_FILE* %20, %struct._IO_FILE** @trace_file, align 8
  br label %21

21:                                               ; preds = %7, %11, %8
  ret void
}

; Function Attrs: nounwind
declare i32 @pthread_mutex_lock(%union.pthread_mutex_t* noundef) #3

; Function Attrs: nounwind
declare i32 @sprintf(i8* noundef, i8* noundef, ...) #3

; Function Attrs: nounwind
declare i32 @pthread_mutex_unlock(%union.pthread_mutex_t* noundef) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @printMemoryAccess(i64 noundef %0) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load %struct._IO_FILE*, %struct._IO_FILE** @trace_file, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call i64 @va2pa(i64 noundef %4)
  %6 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* noundef %3, i8* noundef getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i64 0, i64 0), i64 noundef %5)
  ret void
}

declare i32 @fprintf(%struct._IO_FILE* noundef, i8* noundef, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @va2pa(i64 noundef %0) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i64, align 8
  %9 = alloca i8, align 1
  store i64 %0, i64* %3, align 8
  %10 = load i64, i64* %3, align 8
  %11 = icmp uge i64 %10, 4503599627367936
  br i1 %11, label %12, label %17

12:                                               ; preds = %1
  %13 = load i64, i64* %3, align 8
  %14 = icmp ult i64 %13, 4503599627370495
  br i1 %14, label %15, label %17

15:                                               ; preds = %12
  %16 = load i64, i64* %3, align 8
  store i64 %16, i64* %2, align 8
  br label %65

17:                                               ; preds = %12, %1
  %18 = load i32, i32* @fd_va, align 4
  %19 = icmp eq i32 %18, -1
  br i1 %19, label %20, label %27

20:                                               ; preds = %17
  %21 = call i32 (i8*, i32, ...) @open(i8* noundef getelementptr inbounds ([19 x i8], [19 x i8]* @.str.6, i64 0, i64 0), i32 noundef 0)
  store i32 %21, i32* @fd_va, align 4
  %22 = load i32, i32* @fd_va, align 4
  %23 = icmp ne i32 %22, -1
  br i1 %23, label %24, label %25

24:                                               ; preds = %20
  br label %26

25:                                               ; preds = %20
  call void @__assert_fail(i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.8, i64 0, i64 0), i32 noundef 32, i8* noundef getelementptr inbounds ([25 x i8], [25 x i8]* @__PRETTY_FUNCTION__.va2pa, i64 0, i64 0)) #7
  unreachable

26:                                               ; preds = %24
  br label %27

27:                                               ; preds = %26, %17
  %28 = load i64, i64* %3, align 8
  %29 = mul i64 %28, 8
  store i64 %29, i64* %4, align 8
  %30 = load i32, i32* @fd_va, align 4
  %31 = load i64, i64* %4, align 8
  %32 = call i64 @lseek64(i32 noundef %30, i64 noundef %31, i32 noundef 0) #6
  store i64 %32, i64* %5, align 8
  %33 = load i64, i64* %5, align 8
  %34 = load i64, i64* %4, align 8
  %35 = icmp eq i64 %33, %34
  br i1 %35, label %36, label %37

36:                                               ; preds = %27
  br label %38

37:                                               ; preds = %27
  call void @__assert_fail(i8* noundef getelementptr inbounds ([16 x i8], [16 x i8]* @.str.9, i64 0, i64 0), i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.8, i64 0, i64 0), i32 noundef 36, i8* noundef getelementptr inbounds ([25 x i8], [25 x i8]* @__PRETTY_FUNCTION__.va2pa, i64 0, i64 0)) #7
  unreachable

38:                                               ; preds = %36
  %39 = load i32, i32* @fd_va, align 4
  %40 = bitcast i64* %6 to i8*
  %41 = call i64 @read(i32 noundef %39, i8* noundef %40, i64 noundef 8)
  store i64 %41, i64* %7, align 8
  %42 = load i64, i64* %7, align 8
  %43 = icmp ne i64 %42, 8
  br i1 %43, label %44, label %46

44:                                               ; preds = %38
  %45 = load i64, i64* %3, align 8
  store i64 %45, i64* %2, align 8
  br label %65

46:                                               ; preds = %38
  %47 = load i64, i64* %6, align 8
  %48 = and i64 %47, 18014398509481983
  store i64 %48, i64* %8, align 8
  %49 = load i64, i64* %6, align 8
  %50 = ashr i64 %49, 63
  %51 = and i64 %50, 1
  %52 = icmp ne i64 %51, 0
  %53 = zext i1 %52 to i8
  store i8 %53, i8* %9, align 1
  %54 = load i8, i8* %9, align 1
  %55 = trunc i8 %54 to i1
  br i1 %55, label %58, label %56

56:                                               ; preds = %46
  %57 = load i64, i64* %3, align 8
  store i64 %57, i64* %2, align 8
  br label %65

58:                                               ; preds = %46
  %59 = load i64, i64* %8, align 8
  %60 = icmp ne i64 %59, 0
  br i1 %60, label %61, label %62

61:                                               ; preds = %58
  br label %63

62:                                               ; preds = %58
  call void @__assert_fail(i8* noundef getelementptr inbounds ([9 x i8], [9 x i8]* @.str.10, i64 0, i64 0), i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.8, i64 0, i64 0), i32 noundef 63, i8* noundef getelementptr inbounds ([25 x i8], [25 x i8]* @__PRETTY_FUNCTION__.va2pa, i64 0, i64 0)) #7
  unreachable

63:                                               ; preds = %61
  %64 = load i64, i64* %8, align 8
  store i64 %64, i64* %2, align 8
  br label %65

65:                                               ; preds = %63, %56, %44, %15
  %66 = load i64, i64* %2, align 8
  ret i64 %66
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @printBranchCondition(i1 noundef zeroext %0) #0 {
  %2 = alloca i8, align 1
  %3 = zext i1 %0 to i8
  store i8 %3, i8* %2, align 1
  %4 = load i8, i8* %2, align 1
  %5 = trunc i8 %4 to i1
  br i1 %5, label %6, label %9

6:                                                ; preds = %1
  %7 = load %struct._IO_FILE*, %struct._IO_FILE** @trace_file, align 8
  %8 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* noundef %7, i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.4, i64 0, i64 0))
  br label %12

9:                                                ; preds = %1
  %10 = load %struct._IO_FILE*, %struct._IO_FILE** @trace_file, align 8
  %11 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* noundef %10, i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i64 0, i64 0))
  br label %12

12:                                               ; preds = %9, %6
  ret void
}

declare i32 @open(i8* noundef, i32 noundef, ...) #2

; Function Attrs: noreturn nounwind
declare void @__assert_fail(i8* noundef, i8* noundef, i32 noundef, i8* noundef) #4

; Function Attrs: nounwind
declare i64 @lseek64(i32 noundef, i64 noundef, i32 noundef) #3

declare i64 @read(i32 noundef, i8* noundef, i64 noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind readnone willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { noreturn nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind readnone willreturn }
attributes #6 = { nounwind }
attributes #7 = { noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
