(require 'compile)

(setq init-script-path (convert-standard-filename (file-name-directory load-file-name)))
(setq project-root-path init-script-path)
(setq execution-root-path (concat project-root-path "builds"))
(setq build-output-path (concat project-root-path "builds"))
(setq source-path (concat project-root-path "source"))
(setq build-script-path (concat project-root-path "build.bat"))

(defun build-command (build-type)
  (setq build-command (concat build-script-path " " source-path " " build-output-path " " build-type)))
(defun run-command (build-type)
  (concat "cd " execution-root-path " && " build-output-path "\\" "test_" build-type ".exe" "&"))

;; saves everything and recompiles the debug build
(global-set-key (kbd "<f1>")
                (lambda ()
                  (interactive)
                  (setq compile-command (build-command "debug"))
                  (setq compilation-finish-functions nil)
                  (save-some-buffers 1) ; save all buffers with changes
                  (recompile)))

;; saves everything and recompiles the release build
(global-set-key (kbd "<f2>")
                (lambda ()
                  (interactive)
                  (setq compile-command (build-command "release"))
                  (setq compilation-finish-functions nil)
                  (save-some-buffers 1) ; save all buffers with changes
                  (recompile)))

;; executes the debug program
(global-set-key (kbd "<f5>")
		(lambda ()
		  (interactive)
		  (shell-command (run-command "debug"))))

;; executes the release program
(global-set-key (kbd "<f6>")
		(lambda ()
		  (interactive)
		  (shell-command (run-command "release"))))
