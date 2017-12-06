import React from 'react';
import BasePage from '../../components/BasePage'
import { connect } from 'react-redux';
import { Tabs } from 'antd';
import CodeComponent from './_code';

class Page extends BasePage {
    componentDidMount() {
        this.props.dispatch({
            type: 'repository/tree',
            payload: {
                repository: this.props.match.params.repository,
                flag: 'master',
                path: ''
            }
        });
        this.props.dispatch({
            type: 'repository/repositoryBranch',
            payload: {
                repository: this.props.match.params.repository
            }
        });
    }

    page() {
        return <div>
            <h2>{ this.props.match.params.repository }</h2>
            <Tabs defaultActiveKey={ this.props.match.params.tag }>
                <Tabs.TabPane tab={ <span>Code</span> } key="code">
                    <CodeComponent { ...this.props } />
                </Tabs.TabPane>
                <Tabs.TabPane tab={ <span>Issue</span> } key="issue">
                    <span>TODO</span>
                </Tabs.TabPane>
            </Tabs>
        </div>
    }
}

export default connect(
    ({ repository }) => ({
        fs: repository.fs,
        file: repository.file,
        singleDisplayType: repository.singleDisplayType,
        branches: repository.branch
    })
)(Page);