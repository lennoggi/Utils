#!/bin/bash

# Parameters
myfiles=(
    "/scratch/07825/lennoggi/BBH_handoff_McLachlan_pp08_large_14rl_NewCooling/output-0007/BBH_handoff_McLachlan_pp08_large_14rl_NewCooling.out"
    "/scratch/07825/lennoggi/BBH_handoff_McLachlan_pm08_large_14rl_NewCooling/output-0008/BBH_handoff_McLachlan_pm08_large_14rl_NewCooling.out"
    "/scratch/07825/lennoggi/BBH_handoff_McLachlan_mm08_large_14rl_NewCooling/output-0008/BBH_handoff_McLachlan_mm08_large_14rl_NewCooling.out"
)
check_every_seconds=300  # 5 minutes
mail_file="/var/spool/mail/lennoggi"


# Monitor the files
while true
do
    for myfile in "${myfiles[@]}"
    do
        if [ -f "${myfile}" ]; then
            last_modified_time=$(stat -c %Y "${myfile}")
            current_time=$(date +%s)
            time_since_update=$((current_time - last_modified_time))

            if [ "${time_since_update}" -gt "${check_every_seconds}" ]; then
                subject="Job monitor alert: potential hang"
                message="File '${myfile}' on $(hostname) has not been updated for the last $((check_every_seconds/60)) minutes.\n\n"
                message+="Last updated: $(date -d @${last_modified_time} '+%Y-%m-%d %H:%M:%S')\n"
                message+="Current time: $(date '+%Y-%m-%d %H:%M:%S')\n"
                echo -e "Subject:${subject}\n\n${message}" | sendmail "${USER}@localhost"
            else
                echo -e "\nFile '${myfile}' is active. Last updated: $(date -d @${last_modified_time} '+%Y-%m-%d %H:%M:%S')"
            fi
        else
            echo "Output file ${myfile} does not exist."
        fi
    done

    sleep "${check_every_seconds}"
done
