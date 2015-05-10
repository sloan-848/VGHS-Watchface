var gulp = require('gulp');
var gutil = require('gulp-util');
var exec = require('child_process').exec;

gulp.task('build', function(cb){
  exec('pebble build', function(err, stdout, stderr){
    gutil.log(stdout);
    gutil.log(stderr);
    cb(err);
  });
});

var installOptions = '--logs';

gulp.task('install', ['build'], function(){
  exec('pebble install', function(err, stdout, stderr){
    gutil.log(stdout);
    gutil.log(stderr);
    //event(err);
  });
});

gulp.task('watch', function(){
  gulp.watch(['src/**/*', '!src/**/*.swp'], ['install']);
});

gulp.task('default', ['watch', 'install']);
